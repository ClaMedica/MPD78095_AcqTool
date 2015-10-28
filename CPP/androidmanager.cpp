/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "androidmanager.h"
#include <QDebug>
#ifdef ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#endif
AndroidManager::AndroidManager(QObject *parent)
    : QObject(parent)
{
}

AndroidManager &AndroidManager::instance(QObject *parent)
{
    static AndroidManager androidManager(parent);
    return androidManager;
}

void AndroidManager::showNotification(QString __title,QString __message)
{
#ifdef ANDROID
    QAndroidJniObject title=QAndroidJniObject::fromString(__title);
    QAndroidJniObject message=QAndroidJniObject::fromString(__message);
    QString signature="("+getSignature("jstring")+getSignature("jstring")+")V";
    QtAndroid::androidActivity().callMethod<void>(
                "showNotification",
                signature.toLatin1().data(),
                title.object<jstring>(),message.object<jstring>());
#endif
}

void AndroidManager::onPowerConnected()
{
    qDebug()<<"Power connected";
}

void AndroidManager::onPowerDisconnected()
{
    qDebug()<<"Power disconnected";
}

void AndroidManager::onAirplaneModeChanged()
{
    qDebug()<<"Cambio modalità aereo";
}

void AndroidManager::launchService(QString __serviceAction)
{
#ifdef ANDROID
    QAndroidJniObject string=QAndroidJniObject::fromString(__serviceAction);
    QtAndroid::androidActivity().callMethod<void>(
                "launchService",
                convertSignature("jstring#void"),
                string.object<jstring>());


#endif
}

void AndroidManager::showToast(QString __s)
{
#ifdef ANDROID
    qDebug()<<"toast"<<__s;
    QAndroidJniObject string=QAndroidJniObject::fromString(__s);


    QtAndroid::androidActivity().callMethod<void>(
                "showToast",
                convertSignature("jstring#void"),//sto metodo è sofisticato, vuole un const char*
                string.object<jstring>());

#endif
}

void AndroidManager::killService()
{
#ifdef ANDROID
    QtAndroid::androidActivity().callMethod<void>(
                "killService",
                "()V");
#endif
}


void AndroidManager::updateAndroidNotification()
{
#ifdef ANDROID
    QtAndroid::androidActivity().callMethod<void>(
                "killService",
                "()V");
#endif
}

const char* AndroidManager::convertSignature(QString __original)
{
    //la firma originale ha gli argomenti separati da @ e il return lo trovi dopo #
    QStringList parameters=__original.split("#").at(0).split("@");
    QString ret;
    ret.append("(");
    foreach (QString p, parameters) {
        ret.append(getSignature(p));
    }
    ret.append(")");
    if(getSignature(__original.split("#").at(1))=="")
        ret.append("V");
    else
        ret.append(getSignature(__original.split("#").at(1)));

    return ret.toLatin1().data();
}

QString AndroidManager::getSignature(QString __type)
{

         if(__type=="jboolean")      {return "Z";}
    else if(__type=="jbyte")         {return "B";}
    else if(__type=="jchar")         {return "C";}
    else if(__type=="jshort")        {return "S";}
    else if(__type=="jint")          {return "I";}
    else if(__type=="jlong")         {return "J";}
    else if(__type=="jfloat")        {return "F";}
    else if(__type=="jdouble")       {return "D";}
    else if(__type=="jobject")       {return "Ljava/lang/Object;";}
    else if(__type=="jclass")        {return "Ljava/lang/Class;";}
    else if(__type=="jstring")       {return "Ljava/lang/String;";}
    else if(__type=="jthrowable")    {return "Ljava/lang/Throwable;";}
    else if(__type=="jobjectArray")  {return "[Ljava/lang/Object;";}
    //else if(__type=="jarray")        {return "[<type>";}
    else if(__type=="jbooleanArray") {return "[Z";}
    else if(__type=="jbyteArray")    {return "[B";}
    else if(__type=="jcharArray")    {return "[C";}
    else if(__type=="jshortArray")   {return "[S";}
    else if(__type=="jintArray")     {return "[I";}
    else if(__type=="jlongArray")    {return "[J";}
    else if(__type=="jfloatArray")   {return "[F";}
    else if(__type=="jdoubleArray")  {return "[D";}

    return "";
}

