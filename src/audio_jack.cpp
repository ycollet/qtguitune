//
//    guitune - program for tuning instruments (actually an oscilloscope)
//    Copyright (C) 1999  Florian Berger
//    Copyright (C) 2013-2015  Yann Collette
//    Email: florian.berger@jk.uni-linz.ac.at
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License Version 2 as
//    published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifdef ENABLE_JACK

#include <math.h>
#include <iostream>

#include <QtWidgets>

#include "audio_jack.h"
#include "resources.h"
#include "guitune.h"
#include "osziview.h"
#include "logview.h"

extern double KAMMERTON, KAMMERTON_LOG;

AudioJACK::AudioJACK(QString audio_name)
    : AudioBase(audio_name), client(nullptr), input_port(nullptr), ringbuffer(nullptr)
{
    freqs[0]  = KAMMERTON;
    lfreqs[0] = KAMMERTON_LOG;
    for (int i = 1; i < 12; i++) {
        freqs[i]  = freqs[i-1]  * D_NOTE;
        lfreqs[i] = lfreqs[i-1] + D_NOTE_LOG;
    }
}

AudioJACK::~AudioJACK()
{
    close_audio();
}

int AudioJACK::init_audio()
{
    jack_status_t status;
    std::cout << qPrintable(tr("initializing JACK audio with client name: "))
              << qPrintable(dsp_devicename) << std::endl;

    client = jack_client_open(dsp_devicename.toStdString().c_str(), JackNullOption, &status);
    if (!client) {
        std::cerr << "Failed to open JACK client (status=0x" << std::hex << status << std::dec << ")" << std::endl;
        return -1;
    }

    // JACK controls the sample rate; update our local copy
    sampfreq = (int)jack_get_sample_rate(client);
    std::cout << "JACK sample rate: " << sampfreq << " Hz" << std::endl;

    input_port = jack_port_register(client, "input",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    if (!input_port) {
        std::cerr << "Failed to register JACK input port" << std::endl;
        jack_client_close(client);
        client = nullptr;
        return -1;
    }

    ringbuffer = jack_ringbuffer_create(JACK_RINGBUF_SIZE * sizeof(float));
    if (!ringbuffer) {
        std::cerr << "Failed to create JACK ring buffer" << std::endl;
        jack_client_close(client);
        client = nullptr;
        return -1;
    }

    jack_set_process_callback(client, jack_process_cb, this);

    if (jack_activate(client) != 0) {
        std::cerr << "Failed to activate JACK client" << std::endl;
        jack_ringbuffer_free(ringbuffer);
        ringbuffer = nullptr;
        jack_client_close(client);
        client = nullptr;
        return -1;
    }

    blksize = 256;
    return 1;
}

int AudioJACK::close_audio()
{
    if (client) {
        jack_deactivate(client);
        jack_client_close(client);
        client = nullptr;
        input_port = nullptr;
    }
    if (ringbuffer) {
        jack_ringbuffer_free(ringbuffer);
        ringbuffer = nullptr;
    }
    return 0;
}

void AudioJACK::setDSPName(QString name)
{
    close_audio();
    dsp_devicename = name;
    init_audio();
}

void AudioJACK::setSampleFreq(int /*f*/)
{
    // JACK sample rate is controlled by the server; query it instead of setting
    if (client)
        sampfreq = (int)jack_get_sample_rate(client);
}

int AudioJACK::jack_process_cb(jack_nframes_t nframes, void *arg)
{
    return static_cast<AudioJACK*>(arg)->process(nframes);
}

int AudioJACK::process(jack_nframes_t nframes)
{
    if (!input_port || !ringbuffer)
        return 0;

    jack_default_audio_sample_t *in =
        (jack_default_audio_sample_t*)jack_port_get_buffer(input_port, nframes);

    size_t bytes = nframes * sizeof(jack_default_audio_sample_t);
    if (jack_ringbuffer_write_space(ringbuffer) >= bytes)
        jack_ringbuffer_write(ringbuffer, (const char*)in, bytes);

    return 0;
}

void AudioJACK::proc_audio()
{
    if (!client || !ringbuffer)
        return;

    int i, j, n, trig, trigpos;
    unsigned char *c = sample;
    double ldf, mldf;
    float fbuf[256];

    processing_audio = 1;
    trigpos = 0;
    c = sample;

    // Read blksize float samples from the ring buffer and convert to unsigned char.
    // JACK delivers float [-1.0, 1.0]; we map to [0, 255] with 128 as silence.
    auto read_block = [&]() {
        size_t needed = (size_t)blksize * sizeof(float);
        while (jack_ringbuffer_read_space(ringbuffer) < needed)
            QThread::msleep(1);
        jack_ringbuffer_read(ringbuffer, (char*)fbuf, needed);
        for (int k = 0; k < blksize; k++) {
            float s = fbuf[k];
            if (s >  1.0f) s =  1.0f;
            if (s < -1.0f) s = -1.0f;
            c[k] = (unsigned char)((s + 1.0f) * 127.5f + 0.5f);
        }
    };

    read_block();
    n = blksize;

    for (i = 0; i < n && Abs(c[i] - 128) < 2; i++);

    j = 0;
    trig = 0;

    if (i < n) do {
        for (; i < n-1; i++)
            if (POSTRIG(c, i)) {
                trig    = 1;
                trigpos = i;
            }
        if (trig == 0) {
            read_block();
            n = blksize;
            j++;
            i = 0;
        }
    } while (!trig && j < NO_TRIG_LIMIT);

    if (trig) {
        for (i = n - trigpos; i < sampnr; i += n) {
            c += n;
            read_block();
            n = blksize;
        }

        mw->getOsziPtr()->setSamplePtr(&sample[trigpos]);
        mw->getOsziPtr()->paintSample();

        freq_0t  = (double)sampfreq * mw->getOsziPtr()->getfreq2();
        lfreq_0t = log(freq_0t);

        while (lfreq_0t <  lfreqs[0]         - D_NOTE_LOG/2.0) lfreq_0t += LOG_2;
        while (lfreq_0t >= lfreqs[0] + LOG_2 - D_NOTE_LOG/2.0) lfreq_0t -= LOG_2;

        mldf    = D_NOTE_LOG;
        note_0t = 0;

        for (i = 0; i < 12; i++) {
            ldf = fabs(lfreq_0t - lfreqs[i]);
            if (ldf < mldf) {
                mldf    = ldf;
                note_0t = i;
            }
        }

        update_lfreq();

        double nfreq_0t = freqs[note_0t];
        while (nfreq_0t / freq_0t > D_NOTE_SQRT) nfreq_0t /= 2.0;
        while (freq_0t / nfreq_0t > D_NOTE_SQRT) nfreq_0t *= 2.0;

        update_nfreq(nfreq_0t);
    }

    processing_audio = 0;
}

#endif // ENABLE_JACK
