/*============================================================================
 * audio.cpp
 *      Code to interface to windows direct sound to control the audio
 *      hardware.
 *==========================================================================*/

// Have to do this define to get SwitchToThread()
#define _WIN32_WINNT            0x0400

#include "mex.h"

#include "Windows.h"
#include <Dsound.h>

/*============================================================================
 * audioPlay
 *      Plays a buffer of audio data.
 *==========================================================================*/
int audioPlay(signed short *data, unsigned int samples, unsigned int sampleRate)
{
    HRESULT result;
    unsigned int i;
    int ccode;
    void *pLockBuff;
    signed short int *s;
    
    DWORD lockSize;
    DWORD bufferSize;
    LPDIRECTSOUND pds;
    DSBUFFERDESC bdesc;
    LPDIRECTSOUNDBUFFER pbuff;
    LPDIRECTSOUNDBUFFER sbuff;    
    WAVEFORMATEX  wfxOutput;

    DWORD status;
    
    /* Create the direct sound object */
    if ((result = DirectSoundCreate(NULL, /* DSDEVID_DefaultPlayback, */
                                    &pds,
                                    NULL)) != DS_OK)
    {
        mexPrintf("DirectSoundCreate() returned %x.\n", result);
        ccode = -1;
        goto backout_base;
    }

    /* Set the cooperative level to PRIORITY */
    if ((result = pds->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)) != DS_OK)
    {
        mexPrintf("SetCooperativeLevel() returned %x.\n", result);
        ccode = -2;
        goto backout_direct_sound;
    }

    /* Get reference to primary buffer. */
    memset(&bdesc, 0, sizeof(bdesc));
    bdesc.dwSize = sizeof(DSBUFFERDESC);
    bdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if ((result = pds->CreateSoundBuffer(&bdesc, &pbuff, NULL)) != DS_OK)
    {
        mexPrintf("CreateSoundBuffer() returned %x.\n", result);
        ccode = -3;
        goto backout_pri_buffer;
    }

    /* Set up the format of the output buffer */
    memset(&wfxOutput, 0, sizeof(wfxOutput));
    wfxOutput.wFormatTag = WAVE_FORMAT_PCM;
    wfxOutput.nSamplesPerSec = sampleRate;
    wfxOutput.nChannels = 2;
    wfxOutput.wBitsPerSample = 16;
    wfxOutput.nBlockAlign = wfxOutput.nChannels * ( wfxOutput.wBitsPerSample / 8 );
    wfxOutput.nAvgBytesPerSec = wfxOutput.nBlockAlign * wfxOutput.nSamplesPerSec;
    
    /* Create the secondary buffer. */
    bufferSize = samples*sizeof(signed short)*2;
    memset(&bdesc, 0, sizeof(bdesc));
    bdesc.dwSize = sizeof(DSBUFFERDESC);
    bdesc.dwFlags         = DSBCAPS_GLOBALFOCUS;
    bdesc.dwBufferBytes   = bufferSize;
    // bdesc.guid3DAlgorithm = GUID_NULL;
    bdesc.lpwfxFormat     = &wfxOutput;

    /* Create secondary buffer */
    if ((result = pds->CreateSoundBuffer(&bdesc, &sbuff, NULL)) != DS_OK)
    {
        mexPrintf("CreateSoundBuffer() secondary returned %x.\n", result);
        ccode = -4;
        goto backout_sec_buffer;
    }

    /* Set te format of the primary buffer */
    if ((result = pbuff->SetFormat(&wfxOutput)) != DS_OK)
    {
        mexPrintf("SetFormat() returned %x.\n", result);
        ccode = -5;
        goto backout_sec_buffer;
    }

    /* Lock the buffer and fill with the first chunk of samples */
    if ((result = sbuff->Lock(0, bufferSize, &pLockBuff, &lockSize, NULL, NULL, 0)) != DS_OK)
    {
        mexPrintf("Lock() returned %x.\n", result);
        ccode = -6;
        goto backout_sec_buffer;
    }

    //mexPrintf("buffsize=%d locksize=%d.\n", bufferSize, lockSize);
    
    /* Put the samples in */
    s = (signed short *) pLockBuff;
    for (i=0; i<samples*2; i++)
    {
        s[i] = data[i];
    }
    
    /* Unlock the buffer */
    sbuff->Unlock(&pLockBuff, lockSize, NULL, NULL);
    
    /* Play the sound */
    sbuff->Play(0, 0, NULL);

    /* Poll for completion */
    for ( ; ; )
    {
        sbuff->GetStatus(&status);
        if (status != DSBSTATUS_PLAYING)
        {
            break;
        }
        SwitchToThread();
    }

    sbuff->Stop();
    
    /* Done */
    ccode = 0;

 backout_sec_buffer:
    sbuff->Release();
    
 backout_pri_buffer:
    pbuff->Release();
    
 backout_direct_sound:
    pds->Release();
        
 backout_base:
    return(ccode);
}

/*----------------------------------------------------------------------------
 * audioRecord
 *      Records a section of audio from the soundcard.
 *--------------------------------------------------------------------------*/
int audioRecord(signed short *data, unsigned int samples, unsigned int sampleRate)
{
    HRESULT result;
    unsigned int i;
    int ccode;
    void *pLockBuff;
    signed short int *s;
    
    DWORD lockSize;
    DWORD bufferSize;
    LPDIRECTSOUNDCAPTURE pdsc;
    DSCBUFFERDESC bdesc;
    LPDIRECTSOUNDCAPTUREBUFFER scbuff;    
    WAVEFORMATEX  wfxInput;

    DWORD status;
    
    /* Create the direct sound object */
    if ((result = DirectSoundCaptureCreate(NULL, /* DSDEVID_DefaultCapture, */
                                    &pdsc,
                                    NULL)) != DS_OK)
    {
        mexPrintf("DirectSoundCaptureCreate() returned %x.\n", result);
        ccode = -1;
        goto backout_base;
    }

    /* Set up the format of the capture buffer */
    memset(&wfxInput, 0, sizeof(wfxInput));
    wfxInput.wFormatTag = WAVE_FORMAT_PCM;
    wfxInput.nSamplesPerSec = sampleRate;
    wfxInput.nChannels = 2;
    wfxInput.wBitsPerSample = 16;
    wfxInput.nBlockAlign = wfxInput.nChannels * ( wfxInput.wBitsPerSample / 8 );
    wfxInput.nAvgBytesPerSec = wfxInput.nBlockAlign * wfxInput.nSamplesPerSec;
    
    /* Create the secondary buffer. */
    bufferSize = samples*sizeof(signed short)*2;
    memset(&bdesc, 0, sizeof(bdesc));
    bdesc.dwSize = sizeof(DSCBUFFERDESC);
    bdesc.dwFlags         = 0;
    bdesc.dwBufferBytes   = bufferSize;
    // bdesc.guid3DAlgorithm = GUID_NULL;
    bdesc.lpwfxFormat     = &wfxInput;

    /* Create secondary buffer */
    if ((result = pdsc->CreateCaptureBuffer(&bdesc, &scbuff, NULL)) != DS_OK)
    {
        mexPrintf("CreateCaptureBuffer() secondary returned %x.\n", result);
        ccode = -4;
        goto backout_direct_sound;
    }

    /* Record */
    scbuff->Start(NULL);

    /* Poll for completion */
    for ( ; ; )
    {
        scbuff->GetStatus(&status);
        // mexPrintf("status=%d\n", status);
        if (status != DSCBSTATUS_CAPTURING)
        {
            break;
        }
        SwitchToThread();
    }

    /* Lock the buffer */
    if ((result = scbuff->Lock(0, bufferSize, &pLockBuff, &lockSize, NULL, NULL, 0)) != DS_OK)
    {
        mexPrintf("Lock() returned %x.\n", result);
        ccode = -6;
        goto backout_sec_buffer;
    }

    //mexPrintf("bufferSize=%d, lockSize=%d.\n", bufferSize, lockSize);
    
    /* Copy the samples */
    s = (signed short *) pLockBuff;
    for (i=0; i<samples*2; i++)
    {
        data[i] = s[i];
    }
    
    /* Unlock the buffer */
    scbuff->Unlock(&pLockBuff, lockSize, NULL, NULL);
    
    /* Done */
    ccode = 0;

 backout_sec_buffer:
    scbuff->Release();
    
 backout_direct_sound:
    pdsc->Release();
        
 backout_base:
    return(ccode);
}

/*============================================================================
 * audioRecPlay
 *      Records and plays at the same time.
 *==========================================================================*/
int audioRecPlay(signed short *data_play, signed short *data_rec, unsigned int samples, unsigned int sampleRate)
{
    HRESULT result;
    unsigned int i;
    int ccode;
    void *pLockBuff;
    signed short int *s;
    
    DWORD lockSize;
    DWORD bufferSize;

    /* Playback */
    LPDIRECTSOUND pds;
    DSBUFFERDESC bdesc;
    LPDIRECTSOUNDBUFFER pbuff;
    LPDIRECTSOUNDBUFFER sbuff;    
    WAVEFORMATEX  wfxOutput;

    /* Record */
    LPDIRECTSOUNDCAPTURE pdsc;
    DSCBUFFERDESC bcdesc;
    LPDIRECTSOUNDCAPTUREBUFFER scbuff;    
    WAVEFORMATEX  wfxInput;

    DWORD status;

    /*-- Playback --------------------------------------------------------------*/
    
    /* Create the direct playback sound object */
    if ((result = DirectSoundCreate(NULL, /* DSDEVID_DefaultPlayback, */
                                    &pds,
                                    NULL)) != DS_OK)
    {
        mexPrintf("DirectSoundCreate() returned %x.\n", result);
        ccode = -1;
        goto backout_base;
    }

    /* Set the cooperative level to PRIORITY */
    if ((result = pds->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)) != DS_OK)
    {
        mexPrintf("SetCooperativeLevel() returned %x.\n", result);
        ccode = -2;
        goto backout_direct_sound;
    }

    /* Get reference to primary buffer. */
    memset(&bdesc, 0, sizeof(bdesc));
    bdesc.dwSize = sizeof(DSBUFFERDESC);
    bdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if ((result = pds->CreateSoundBuffer(&bdesc, &pbuff, NULL)) != DS_OK)
    {
        mexPrintf("CreateSoundBuffer() returned %x.\n", result);
        ccode = -3;
        goto backout_direct_sound;
    }

    /* Set up the format of the output buffer */
    memset(&wfxOutput, 0, sizeof(wfxOutput));
    wfxOutput.wFormatTag = WAVE_FORMAT_PCM;
    wfxOutput.nSamplesPerSec = sampleRate;
    wfxOutput.nChannels = 2;
    wfxOutput.wBitsPerSample = 16;
    wfxOutput.nBlockAlign = wfxOutput.nChannels * ( wfxOutput.wBitsPerSample / 8 );
    wfxOutput.nAvgBytesPerSec = wfxOutput.nBlockAlign * wfxOutput.nSamplesPerSec;
    
    /* Create the secondary buffer. */
    bufferSize = samples*sizeof(signed short)*2;
    memset(&bdesc, 0, sizeof(bdesc));
    bdesc.dwSize = sizeof(DSBUFFERDESC);
    bdesc.dwFlags         = DSBCAPS_GLOBALFOCUS;
    bdesc.dwBufferBytes   = bufferSize;
    // bdesc.guid3DAlgorithm = GUID_NULL;
    bdesc.lpwfxFormat     = &wfxOutput;

    /* Create secondary buffer */
    if ((result = pds->CreateSoundBuffer(&bdesc, &sbuff, NULL)) != DS_OK)
    {
        mexPrintf("CreateSoundBuffer() secondary returned %x.\n", result);
        ccode = -4;
        goto backout_pri_buffer;
    }

    /* Set the format of the primary buffer */
    if ((result = pbuff->SetFormat(&wfxOutput)) != DS_OK)
    {
        mexPrintf("SetFormat() returned %x.\n", result);
        ccode = -5;
        goto backout_sec_buffer;
    }

    /* Lock the buffer and fill with the first chunk of samples */
    if ((result = sbuff->Lock(0, bufferSize, &pLockBuff, &lockSize, NULL, NULL, 0)) != DS_OK)
    {
        mexPrintf("Lock() returned %x.\n", result);
        ccode = -6;
        goto backout_sec_buffer;
    }

    //mexPrintf("buffsize=%d locksize=%d.\n", bufferSize, lockSize);
    
    /* Put the samples in */
    s = (signed short *) pLockBuff;
    for (i=0; i<samples*2; i++)
    {
        s[i] = data_play[i];
    }
    
    /* Unlock the buffer */
    sbuff->Unlock(&pLockBuff, lockSize, NULL, NULL);

    /*-- Record ----------------------------------------------------------------*/

    /* Create the direct sound object */
    if ((result = DirectSoundCaptureCreate(NULL, /* DSDEVID_DefaultCapture, */
                                    &pdsc,
                                    NULL)) != DS_OK)
    {
        mexPrintf("DirectSoundCaptureCreate() returned %x.\n", result);
        ccode = -1;
        goto backout_sec_buffer;
    }

    /* Set up the format of the capture buffer */
    memset(&wfxInput, 0, sizeof(wfxInput));
    wfxInput.wFormatTag = WAVE_FORMAT_PCM;
    wfxInput.nSamplesPerSec = sampleRate;
    wfxInput.nChannels = 2;
    wfxInput.wBitsPerSample = 16;
    wfxInput.nBlockAlign = wfxInput.nChannels * ( wfxInput.wBitsPerSample / 8 );
    wfxInput.nAvgBytesPerSec = wfxInput.nBlockAlign * wfxInput.nSamplesPerSec;
    
    /* Create the secondary buffer. */
    memset(&bcdesc, 0, sizeof(bcdesc));
    bcdesc.dwSize = sizeof(DSCBUFFERDESC);
    bcdesc.dwFlags         = 0;
    bcdesc.dwBufferBytes   = bufferSize;
    bcdesc.lpwfxFormat     = &wfxInput;

    /* Create secondary buffer */
    if ((result = pdsc->CreateCaptureBuffer(&bcdesc, &scbuff, NULL)) != DS_OK)
    {
        mexPrintf("CreateCaptureBuffer() secondary returned %x.\n", result);
        ccode = -4;
        goto backout_cap_direct_sound;
    }

    /*--------------------------------------------------------------------------*/

    /* Start playback */
    sbuff->Play(0, 0, NULL);
    
    /* Start record */
    scbuff->Start(NULL);

    /* Poll for completion of recording. */
    for ( ; ; )
    {
        scbuff->GetStatus(&status);
        if (status != DSCBSTATUS_CAPTURING)
        {
            break;
        }
        SwitchToThread();
    }

    /* Stop playback just to be safe. */
    sbuff->Stop();

    /* Lock the capture buffer */
    if ((result = scbuff->Lock(0, bufferSize, &pLockBuff, &lockSize, NULL, NULL, 0)) != DS_OK)
    {
        mexPrintf("Lock() returned %x.\n", result);
        ccode = -6;
        goto backout_cap_buffer;
    }

    /* Copy the samples */
    s = (signed short *) pLockBuff;
    for (i=0; i<samples*2; i++)
    {
        data_rec[i] = s[i];
    }
    
    /* Unlock the buffer */
    scbuff->Unlock(&pLockBuff, lockSize, NULL, NULL);
    
    /* Done */
    ccode = 0;

 backout_cap_buffer:
    scbuff->Release();
    
 backout_cap_direct_sound:
    pdsc->Release();
    
 backout_sec_buffer:
    sbuff->Release();
    
 backout_pri_buffer:
    pbuff->Release();
    
 backout_direct_sound:
    pds->Release();
        
 backout_base:
    return(ccode);
}


