import torch
import torchaudio
from pyaudio import PyAudio, paInt16 
import numpy as np 
import time
import wave
import torch
from model import get_model
import dlib
import cv2
import numpy as np
import torch.nn.functional as F
import time
import pyttsx3
from difflib import SequenceMatcher
import pyttsx3
import serial

device = torch.device('cuda')
            # detect faces using dlib detector

detector = dlib.get_frontal_face_detector()
# returns dlib's HOG + Linear SVM face detector 
# HOG: https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients

def signal_transmission(pos,engine):
    # Transmit signals to robot to perform certain operations
    serialPort = "COM3"
    baudRate = 57600
    code1 = b'1'
    code2 = b'2'
    if pos == 'BEER':
        ser = serial.Serial(serialPort, baudRate, timeout = 0.5)
        time.sleep(3)
        ser.write(code1)
        print(ser.readline())
        ser.close()
    if pos == 'WHISKY':
        ser = serial.Serial(serialPort, baudRate, timeout = 0.5)
        time.sleep(3)
        ser.write(code2)
        print(ser.readline())
        
        ser.close()
    time.sleep(20)
    script = "now take your bottle,  enjoy your "+pos
    engine.say(script) 
    engine.runAndWait() 
    time.sleep(8)


def get_age(frame,model):
    # get a frame and its age
    #ret, frame = cap.read()
    model.eval()
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    img_h, img_w, _ = np.shape(frame)
    detected = detector(frame,1)
    faces = np.empty((len(detected), 224, 224, 3))
    if len(detected) > 0:
        for i, d in enumerate(detected):
            x1, y1, x2, y2, w, h = d.left(), d.top(), d.right() + 1, d.bottom() + 1, d.width(), d.height()
            xw1 = max(int(x1 - 0.4 * w), 0)
            yw1 = max(int(y1 - 0.4 * h), 0)
            xw2 = min(int(x2 + 0.4 * w), img_w - 1)
            yw2 = min(int(y2 + 0.4 * h), img_h - 1)
            faces[i] = cv2.resize(frame[yw1:yw2 + 1, xw1:xw2 + 1], (224, 224))
        inputs = torch.from_numpy(np.transpose(faces.astype(np.float32), (0, 3, 1, 2))).to(device)
        with torch.no_grad():
            outputs = F.softmax(model(inputs), dim=-1).cpu().data.numpy()
        ages = np.arange(0, 101)
        predicted_ages = (outputs * ages).sum(axis=-1)
        for i, d in enumerate(detected):
            label = "{}".format(int(predicted_ages[i]))
            cv2.putText(frame, label, (d.left(), d.top()))
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        cv2.imshow("result", frame)
        cv2.waitKey(1)

        return predicted_ages

class recoder:
    # recorder for recording the input voice 
    NUM_SAMPLES = 2000      
    SAMPLING_RATE = 16000
    LEVEL = 500       
    COUNT_NUM = 20      
    SAVE_LENGTH = 8         
    TIME_COUNT = 30   

    Voice_String = []

    def savewav(self,filename):
        wf = wave.open(filename, 'wb') 
        wf.setnchannels(1) 
        wf.setsampwidth(2) 
        wf.setframerate(self.SAMPLING_RATE) 
        wf.writeframes(np.array(self.Voice_String).tostring()) 
        wf.close() 
        time.sleep(1)

    def recoder(self):
        pa = PyAudio() 
        stream = pa.open(format=paInt16, channels=1, rate=self.SAMPLING_RATE, input=True, 
            frames_per_buffer=self.NUM_SAMPLES) 
        save_count = 0 
        save_buffer = [] 
        time_count = self.TIME_COUNT

        while True:
            time_count -= 1
            string_audio_data = stream.read(self.NUM_SAMPLES) 
            audio_data = np.fromstring(string_audio_data, dtype=np.short)
            large_sample_count = np.sum( audio_data > self.LEVEL )
            if large_sample_count > self.COUNT_NUM:
                save_count = self.SAVE_LENGTH 
            else: 
                save_count -= 1

            if save_count < 0:
                save_count = 0 

            if save_count > 0 : 
                save_buffer.append( string_audio_data ) 
            else: 
                if len(save_buffer) > 0 : 
                    self.Voice_String = save_buffer
                    save_buffer = [] 
                    return True
            if time_count==0: 
                if len(save_buffer)>0:
                    self.Voice_String = save_buffer
                    save_buffer = [] 
                    return True
                else:
                    return False

class GreedyCTCDecoder(torch.nn.Module):
    def __init__(self, labels, blank=0):
        super().__init__()
        self.labels = labels
        self.blank = blank

    def forward(self, emission: torch.Tensor) -> str:
        indices = torch.argmax(emission, dim=-1)  # [num_seq,]
        indices = torch.unique_consecutive(indices, dim=-1)
        indices = [i for i in indices if i != self.blank]
        return "".join([self.labels[i] for i in indices])

def record(bundle, model):
    r = recoder()
    r.recoder()
    r.savewav("test.wav")
    time.sleep(1)
    waveform, sample_rate = torchaudio.load("test.wav")
    waveform = waveform.to(device)
    # if sample_rate != bundle.sample_rate:
    #     waveform = torchaudio.functional.resample(waveform, sample_rate, bundle.sample_rate)
    try:
        with torch.inference_mode():
            emission, _ = model(waveform)  
    except:
        pass
    decoder = GreedyCTCDecoder(labels=bundle.get_labels())
    transcript = decoder(emission[0])
    return transcript
    
def compare(transcript, compare_targets):
    #compare the words in two sentences for alcohol type
    highest_alike = 0
    word_return = ''
    for word in transcript.split('|'):
        for com in compare_targets:
            sim = similar(com,word)
            if sim>highest_alike:
                highest_alike = sim
                word_return = com
    return word_return


def similar(a, b):
    return SequenceMatcher(None, a, b).ratio()
from pathlib import Path

def main():
    resume_path = Path(__file__).resolve().parent.joinpath("misc", "model.pth")
    checkpoint = torch.load(resume_path, map_location="cuda")
    agemodel = get_model(pretrained = None)
    agemodel.load_state_dict(checkpoint['state_dict'])
    agemodel.to(device)
    engine = pyttsx3.init()
    
    # setting the speaker enginer properties
    rate = engine.getProperty('rate')
    print(rate)
    engine.setProperty('rate', rate-40)
    # slow down the speaker's speed of talking
    engine.setProperty('voice', 'HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Speech\Voices\Tokens\TTS_MS_EN-US_ZIRA_11.0')
    # might not working if this voice is not installed, comment it if it's not working
    
    bundle = torchaudio.pipelines.WAV2VEC2_ASR_BASE_960H
    model = bundle.get_model().to(device)
    cap = cv2.VideoCapture(0)
    while True:
        ret, frame = cap.read()
        detected = detector(frame,1)
        if len(detected) > 0:
            engine.say("what kind of alcohol do you want?") 
            engine.runAndWait() 
            #find any people then
            transcript = record(bundle,model)
            word_return = compare(transcript, ['WHISKY','BEER','NOTHING'])
            script = "do you mean that you want "+word_return
            engine.say(script) 
            engine.runAndWait()
            transcript = record(bundle,model)
            yes_OR_no = compare(transcript, ['YES','NO']) 
            if yes_OR_no == "YES" and word_return!='NOTHING':
                script = 'i need to check if you are eligible for drinking alcohol. Please look at the camera'
                engine.say(script) 
                engine.runAndWait()
                ages = []
                i = 0
                while i<=32:
                    
                    ret = False
                    while ret!=True:
                        ret, frame = cap.read()
                    detected = detector(frame,1)
                    if len(detected) > 0:
                        

                        aa = get_age(frame, agemodel)
                        if type(aa)!=list:
                            i+=1
                            print(i)
                            ages.append(aa[0])
                        else:
                            continue
                avgage = round(sum(ages)/len(ages),0)
                arr_var = np.var(ages)
                # Uncertainty calculated from vairance 
                script = "I assume that you are "+str(int(avgage))+" Years old, with uncertainty "+str(round(arr_var,2))
                engine.say(script) 
                engine.runAndWait()
                if arr_var>=8:
                    script = "I'm not sure about your age, please show ID to the human bar tender"
                    engine.say(script) 
                    engine.runAndWait()    
                    continue  
                if avgage>18:
                    script = "you are above 18, serving "+word_return
                    engine.say(script) 
                    engine.runAndWait()
                    signal_transmission(word_return,engine)
                elif avgage<=18:
                    script = "you are below 18, no alcohol"
                    engine.say(script) 
                    engine.runAndWait()    
                      
            else:
                 
                continue
        else:
            engine.say("put your face in front of screen if You want some alcohol")
            engine.runAndWait()
            time.sleep(5)

            
if __name__ == '__main__':
    main()


