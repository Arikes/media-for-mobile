/*
 * Copyright (c) 2015 Intel Corporation
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "MfM-C-Interface.h"
#include <android/log.h>
#include <string>

#define  LOG_TAG "MfM"
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static JavaVM *s_javaVM = 0;
static jclass s_classID = 0;
static jobject instance = 0;
static std::string s_videoDir;

static jmethodID s_getInstanceMethodID = 0;
static jmethodID s_initCapturingMethodID = 0;
static jmethodID s_startCapturingMethodID = 0;
static jmethodID s_stopCapturingMethodID = 0;
static jmethodID s_getDirectoryDCIMMethodID = 0;

static bool s_isRunning = false;

bool MfM_InitJVM(JavaVM * vm)
{
	LOGD("----- MfM_InitJVM ------");

	JNIEnv *env;
	if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
		LOGD("Can't get the enviroument");
		return false;
	}

	s_javaVM = vm;

	// Search for our class
	jclass clazz = env->FindClass("org/cocos2dx/lib/Capturing");
	if (!clazz) {
		LOGD("Can't find Capturing class");
		return false;
	}
	// Keep a global reference to it
	s_classID = (jclass)env->NewGlobalRef(clazz);

	s_getInstanceMethodID = env->GetStaticMethodID(s_classID, "getInstance", "()Lorg/cocos2dx/lib/Capturing;");
	if (!s_getInstanceMethodID) {
		LOGD("Can't find getInstance() method");
		return false;
	}

    s_initCapturingMethodID = env->GetMethodID(s_classID, "initCapturing", "(IIII)V");
    if (!s_initCapturingMethodID) {
    	LOGD("Can't find initCapturing() method");
        return false;
    }

	s_startCapturingMethodID = env->GetMethodID(s_classID, "startCapturing", "(Ljava/lang/String;)V");
	if (!s_startCapturingMethodID) {
		LOGD("Can't find startCapturing() method");
		return false;
	}

    s_stopCapturingMethodID = env->GetMethodID(s_classID, "stopCapturing", "()V");
    if (!s_stopCapturingMethodID) {
    	LOGD("Can't find stopCapturing() method");
        return false;
    }

    // Register our static method
    s_getDirectoryDCIMMethodID = env->GetStaticMethodID(s_classID, "getDirectoryDCIM", "()Ljava/lang/String;");
    if (!s_getDirectoryDCIMMethodID) {
    	LOGD("Can't find getDirectoryDCIM() static method");
        return false;
    }

    // Get DCIM dir
	jstring value = (jstring)env->CallStaticObjectMethod(s_classID, s_getDirectoryDCIMMethodID);
	const char *res = env->GetStringUTFChars(value, NULL);
	s_videoDir = std::string(res);
	env->ReleaseStringUTFChars(value, res);

	return true;
}

void MfM_StartRecording(int width, int height, int frameRate, int bitRate, const char *videoName)
{
	JNIEnv *env;
	if (s_javaVM->AttachCurrentThread(&env, NULL) < 0) {
		LOGD("AttachCurrentThread failed");
		return;
	}

	if (!instance) {
		instance = env->NewGlobalRef(env->CallStaticObjectMethod(s_classID, s_getInstanceMethodID));
	}

	if (instance) {
		s_isRunning = true;

		// Setup format
		env->CallVoidMethod(instance, s_initCapturingMethodID, width, height, frameRate, bitRate);

		// Start capturing
		std::string videoPath = s_videoDir + videoName;
		jstring string = env->NewStringUTF(videoPath.c_str());
		env->CallVoidMethod(instance, s_startCapturingMethodID, string);
		env->DeleteLocalRef(string);
	}
}

void MfM_StopRecording()
{
	if (instance) {
		JNIEnv *env;
		if (s_javaVM->AttachCurrentThread(&env, NULL) < 0) {
			LOGD("AttachCurrentThread failed");
			return;
		}
		env->CallVoidMethod(instance, s_stopCapturingMethodID);
		s_isRunning = false;
	}
}

bool MfM_isRunning()
{
	return s_isRunning;
}
