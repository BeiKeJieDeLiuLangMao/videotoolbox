# videotoolbox
FFmpeg work correctly:   
```ffmpeg -s 478x850 -i test.yuv -c:v h264_videotoolbox -r 25 ffmpeg-out.mp4```

test.yuv each frame format yuv420:   
<Y 478 * 850 bytes> <U 478 * 850 / 4 bytes> <V 478 * 850 / 4 bytes> ...
