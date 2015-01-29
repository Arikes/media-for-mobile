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

package org.cocos2dx.lib;

import com.intel.inde.mp.IProgressListener;
import com.intel.inde.mp.domain.Resolution;
import com.intel.inde.mp.android.graphics.FullFrameTexture;
import com.intel.inde.mp.android.graphics.FrameBuffer;
import com.intel.inde.mp.android.graphics.EglUtil;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Environment;
import android.util.Log;
import android.content.Context;
import android.graphics.SurfaceTexture;

import java.io.IOException;
import java.io.File;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

public class Capturing
{
	private static final String TAG = "Capturing";
	
	private static FullFrameTexture texture;
	private FrameBuffer frameBuffer;
	private VideoCapture videoCapture;
	private int width = 0;
	private int height = 0;
	private int videoWidth = 0;
	private int videoHeight = 0;
	private int mVideoFrameRate = 30;
	private long mNextCaptureNanoTime = 0;
	private long mStartNanoTime = 0;
	
	private static Capturing instance = null;
	
	private IProgressListener progressListener = new IProgressListener() {
        @Override
        public void onMediaStart() {
        }

        @Override
        public void onMediaProgress(float progress) {
        }

        @Override
        public void onMediaDone() {
        }

        @Override
        public void onMediaPause() {
        }

        @Override
        public void onMediaStop() {
        }

        @Override
        public void onError(Exception exception) {
        }
    };
	
    public Capturing(Context context, int width, int height)
    {
		videoCapture = new VideoCapture(context, progressListener);
		
	    frameBuffer = new FrameBuffer(EglUtil.getInstance());
		frameBuffer.setResolution(new Resolution(width, height));
		this.width = width;
		this.height = height;
		
		texture = new FullFrameTexture();
		
		instance = this;
    }
    
    public static Capturing getInstance()
    {
    	return instance;
    }

    public static String getDirectoryDCIM()
    {
        return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM) + File.separator;
    }

    public void initCapturing(int width, int height, int frameRate, int bitRate)
    {
    	mVideoFrameRate = frameRate;
        VideoCapture.init(width, height, frameRate, bitRate);
        videoWidth = width;
    	videoHeight = height;
    }

    public void startCapturing(String videoPath)
    {
        if (videoCapture == null) {
            return;
        }
        synchronized (videoCapture) {
            try {
            	mStartNanoTime = System.nanoTime();
                videoCapture.start(videoPath);
            } catch (IOException e) {
            }
        }
    }
	
	public void beginCaptureFrame()
    {
    	frameBuffer.bind();
    }
	
	public void captureFrame(int textureID)
    {
        if (videoCapture == null) {
            return;
        }
        synchronized (videoCapture) {
            videoCapture.beginCaptureFrame();
            GLES20.glViewport(0, 0, videoWidth, videoHeight);
            texture.draw(textureID);
            videoCapture.endCaptureFrame();
        }
    }
	
	public void endCaptureFrame()
    {
    	frameBuffer.unbind();
		int textureID = frameBuffer.getTextureId();
		long elapsedNanoTime = System.nanoTime() - mStartNanoTime;
		if (elapsedNanoTime > mNextCaptureNanoTime) {
			captureFrame(textureID);
			mNextCaptureNanoTime += 1000000000 / mVideoFrameRate;
		}
		GLES20.glViewport(0, 0, width, height);
		texture.draw(textureID);
    }

    public void stopCapturing()
    {
        if (videoCapture == null) {
            return;
        }
        synchronized (videoCapture) {
            if (videoCapture.isStarted()) {
                videoCapture.stop();
            }
        }
    }
    
    public boolean isRunning()
    {
    	return videoCapture.isStarted();
    }

}
