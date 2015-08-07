#ifdef ANDROID
#include <jni.h>
#include <QMetaObject>
#include <QMainWindow>
#include <QDebug>
#include "androidmanager.h"

// define our native static functions
// these are the functions that Java part will call directly from Android UI thread
static void onNativePowerDisconnected(JNIEnv * /*env*/, jobject /*obj*/){
    QMetaObject::invokeMethod(&AndroidManager::instance(), "onPowerDisconnected");
}

static void onNativePowerConnected(JNIEnv * /*env*/, jobject /*obj*/){
    QMetaObject::invokeMethod(&AndroidManager::instance(), "onPowerConnected");
}

static void onNativeAirplaneModeChanged(JNIEnv * /*env*/, jobject /*obj*/){
    QMetaObject::invokeMethod(&AndroidManager::instance(), "onAirplaneModeChanged");
}
//create a vector with all our JNINativeMethod(s)
static JNINativeMethod methods[] = {
    {"onNativePowerDisconnected", "()V", (void *)onNativePowerDisconnected},
    {"onNativePowerConnected", "()V", (void *)onNativePowerConnected},
    {"onNativeAirplaneModeChanged", "()V", (void *)onNativeAirplaneModeChanged},
};

// this method is called automatically by Java after the .so file is loaded
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    qDebug()<<"Java JNI_onLoad chiamata correttamente";
    JNIEnv* env;
    // get the JNIEnv pointer.
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        qDebug()<<"Java JNI_onLoad errore sulla getenv";
        return JNI_ERR;
    }

    // search for Java class which declares the native methods
    jclass javaClass = env->FindClass("medica/acqTool/NativeFunctions");
    if (!javaClass)
    {
        qDebug()<<"Java JNI_onLoad errore javaclass non trovata";
        return JNI_ERR;
    }

    // register our native methods
    if (env->RegisterNatives(javaClass, methods,
                             sizeof(methods) / sizeof(methods[0])) < 0)
    {
        qDebug()<<"Java JNI_onLoad errore registrazione metodi";
        return JNI_ERR;
    }
    qDebug()<<"Java JNI_onLoad chiamata con successo";
    return JNI_VERSION_1_6;
}
#endif
