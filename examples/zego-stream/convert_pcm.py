import wave

def convert_pcm_to_wav(pcm_file, wav_file, channels, sample_width, sample_rate):
    # 打开PCM文件
    pcm = open(pcm_file, 'rb')

    # 创建WAV文件
    wav = wave.open(wav_file, 'wb')

    # 设置WAV文件的参数
    wav.setnchannels(channels)
    wav.setsampwidth(sample_width)
    wav.setframerate(sample_rate)

    # 从PCM文件中读取数据，并写入WAV文件
    data = pcm.read()
    wav.writeframes(data)

    # 关闭文件
    pcm.close()
    wav.close()

# 示例用法
pcm_file_path = 'test.pcm'
wav_file_path = 'test.wav'
channels = 1  # 声道数
sample_width = 2  # 样本宽度（字节数）
sample_rate = 16000  # 采样率

convert_pcm_to_wav(pcm_file_path, wav_file_path, channels, sample_width, sample_rate)