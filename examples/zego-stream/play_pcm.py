import wave
import pyaudio

def play_pcm_file(file_path):
    chunk = 1024  # 每次读取的音频数据大小
    wf = wave.open(file_path, 'rb')
    p = pyaudio.PyAudio()

    # 打开音频流
    stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                    channels=wf.getnchannels(),
                    rate=wf.getframerate(),
                    output=True)

    # 读取音频数据并播放
    data = wf.readframes(chunk)
    while data:
        stream.write(data)
        data = wf.readframes(chunk)

    # 关闭音频流和音频文件
    stream.stop_stream()
    stream.close()
    p.terminate()
    wf.close()

play_pcm_file("test.pcm")