#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include "aubio.h"

jfieldID getPtrFieldId(JNIEnv * env, jobject obj)
{
    static jfieldID ptrFieldId = 0;

    if (!ptrFieldId)
    {
        jclass c = (*env)->GetObjectClass(env, obj);
        ptrFieldId = (*env)->GetFieldID(env, c, "ptr", "J");
        (*env)->DeleteLocalRef(env, c);
    }

    return ptrFieldId;
}

jfieldID getInputFieldId(JNIEnv * env, jobject obj)
{
    static jfieldID ptrFieldId = 0;

    if (!ptrFieldId)
    {
        jclass c = (*env)->GetObjectClass(env, obj);
        ptrFieldId = (*env)->GetFieldID(env, c, "input", "J");
        (*env)->DeleteLocalRef(env, c);
    }

    return ptrFieldId;
}

jfieldID getPitchFieldId(JNIEnv * env, jobject obj)
{
    static jfieldID ptrFieldId = 0;

    if (!ptrFieldId)
    {
        jclass c = (*env)->GetObjectClass(env, obj);
        ptrFieldId = (*env)->GetFieldID(env, c, "pitch", "J");
        (*env)->DeleteLocalRef(env, c);
    }

    return ptrFieldId;
}

void Java_ffpetrovic_anrdroid_1aubio_1example_MainActivity_initPitch(JNIEnv * env, jobject obj, jint sampleRate, jint bufferSize)
{
    unsigned int win_s = (unsigned int) bufferSize; // window size
    unsigned int hop_s = win_s / 4; // hop size
    unsigned int samplerate = (unsigned int) sampleRate; // samplerate
    aubio_pitch_t * o = new_aubio_pitch ("yinfft", win_s, hop_s, samplerate);
    fvec_t *input = new_fvec (hop_s); // input buffer
    fvec_t *pitch = new_fvec (1);
//    aubio_pitch_set_silence(o, -10000);
    aubio_pitch_set_tolerance(o, 0.70);
    aubio_pitch_set_unit(o, "Hz");
    (*env)->SetLongField(env, obj, getPtrFieldId(env, obj), (jlong) (o));
    (*env)->SetLongField(env, obj, getInputFieldId(env, obj), (jlong) (input));
    (*env)->SetLongField(env, obj, getPitchFieldId(env, obj), (jlong) (pitch));
}

jfloat Java_ffpetrovic_anrdroid_1aubio_1example_MainActivity_getPitch(JNIEnv * env, jobject obj, jfloatArray inputArray)
{
    aubio_pitch_t * o = (aubio_pitch_t *) (*env)->GetLongField(env, obj, getPtrFieldId(env, obj));
    fvec_t *input = (fvec_t *) (*env)->GetLongField(env, obj, getInputFieldId(env, obj));
    fvec_t *pitch = (fvec_t *) (*env)->GetLongField(env, obj, getPitchFieldId(env, obj)); // input buffer

    jsize len = (*env)->GetArrayLength(env, inputArray);
    if(len != input->length) {
        return len;
    }

    jfloat *body = (*env)->GetFloatArrayElements(env, inputArray, 0);
    // 1. copy inputArray to fvec_t* input (can be optimised)
    for(u_int i = 0; i < len; i++) {
        fvec_set_sample(input, body[i], i);
    }
    (*env)->ReleaseFloatArrayElements(env, inputArray, body, 0);


    float freq = 0;
    if(aubio_silence_detection(input, 45) == 0) {
        aubio_pitch_do (o, input, pitch);
        freq = fvec_get_sample(pitch, 0);
    } else {
        freq = 0;
    }
    return freq;
}

void Java_ffpetrovic_anrdroid_1aubio_1example_MainActivity_cleanupPitch(JNIEnv * env, jobject obj)
{
    aubio_pitch_t * o = (aubio_pitch_t *) (*env)->GetLongField(env, obj, getPtrFieldId(env, obj));
    fvec_t *input = (fvec_t *) (*env)->GetLongField(env, obj, getInputFieldId(env, obj));
    fvec_t *pitch = (fvec_t *) (*env)->GetLongField(env, obj, getPitchFieldId(env, obj)); // input buffer
    del_aubio_pitch (o);
    del_fvec (pitch);
    del_fvec (input);
    aubio_cleanup ();

}

