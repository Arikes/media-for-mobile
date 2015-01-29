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

import android.content.Context;
import com.intel.inde.mp.*;
import com.intel.inde.mp.android.AndroidMediaObjectFactory;
import com.intel.inde.mp.android.AudioFormatAndroid;
import com.intel.inde.mp.android.VideoFormatAndroid;

import java.io.IOException;

public class VideoCapture
{
    private static final String TAG = "VideoCapture";

    private static final String Codec = "video/avc";
    private static int IFrameInterval = 1;

    private static final Object syncObject = new Object();
    private static volatile VideoCapture videoCapture;

    private static VideoFormat videoFormat;
    private static int videoWidth;
    private static int videoHeight;
    private GLCapture capturer;

    private boolean isConfigured;
    private boolean isStarted;
    private long framesCaptured;
	private Context context;
	private IProgressListener progressListener;

    public VideoCapture(Context context, IProgressListener progressListener)
    {
		this.context = context;
        this.progressListener = progressListener;
    }
    
    public static void init(int width, int height, int frameRate, int bitRate)
    {
    	videoWidth = width;
    	videoHeight = height;
    	
    	videoFormat = new VideoFormatAndroid(Codec, videoWidth, videoHeight);
    	videoFormat.setVideoFrameRate(frameRate);
        videoFormat.setVideoBitRateInKBytes(bitRate);
        videoFormat.setVideoIFrameInterval(IFrameInterval);
    }

    public void start(String videoPath) throws IOException
    {
        if (isStarted())
            throw new IllegalStateException(TAG + " already started!");

        capturer = new GLCapture(new AndroidMediaObjectFactory(context), progressListener);
        capturer.setTargetFile(videoPath);
        capturer.setTargetVideoFormat(videoFormat);

        AudioFormat audioFormat = new AudioFormatAndroid("audio/mp4a-latm", 44100, 2);
        capturer.setTargetAudioFormat(audioFormat);

        capturer.start();

        isStarted = true;
        isConfigured = false;
        framesCaptured = 0;
    }    
    
    public void stop()
    {
        if (!isStarted())
            throw new IllegalStateException(TAG + " not started or already stopped!");

        try {
            capturer.stop();
            isStarted = false;
        } catch (Exception ex) {
        }

        capturer = null;
        isConfigured = false;
    }

    private void configure()
    {
        if (isConfigured())
            return;

        try {
            capturer.setSurfaceSize(videoWidth, videoHeight);
            isConfigured = true;
        } catch (Exception ex) {
        }
    }

    public void beginCaptureFrame()
    {
        if (!isStarted())
            return;

        configure();
        if (!isConfigured())
        	return;

        capturer.beginCaptureFrame();
    }

    public void endCaptureFrame()
    {
        if (!isStarted() || !isConfigured())
            return;

        capturer.endCaptureFrame();
        framesCaptured++;
    }

    public boolean isStarted()
    {
        return isStarted;
    }

    public boolean isConfigured()
    {
        return isConfigured;
    }

}
