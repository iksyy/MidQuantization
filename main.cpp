
#include <MidiFile.cpp>
#include <MidiMessage.cpp>
#include <MidiEvent.cpp>
#include <MidiEventList.cpp>
#include <Binasc.cpp>
#include <iostream>
#include <map>

using namespace std;

int quantizeTicks(int ticks, int ppq, int division) {
    int step = ppq / division; // 每个音符类型对应的 tick 数
    return (ticks + step / 2) / step * step; // 四舍五入到最近的 step
}
int getDuration(const smf::MidiEvent& noteOn, const smf::MidiEvent& noteOff) {
    return noteOff.tick - noteOn.tick;
}
void setDuration(smf::MidiEvent& noteOn, smf::MidiEvent& noteOff, int duration) {
    noteOff.tick = noteOn.tick + duration;
}
void quantizeMidi(const string& inputFile, const string& outputFile, int division) {
    smf::MidiFile midiFile;
    midiFile.read(inputFile);
    midiFile.doTimeAnalysis();
    int ppq = midiFile.getTicksPerQuarterNote();

    for (int track = 0; track < midiFile.getTrackCount(); ++track) {
        map<int, smf::MidiEvent*> activeNotes; // 追踪 Note On 事件
        for (int i = 0; i < midiFile[track].size(); ++i) {
            auto& event = midiFile[track][i];
            if (event.isNoteOn()) {
                int quantizedTick = quantizeTicks(event.tick, ppq, division);
                activeNotes[event.getKeyNumber()] = &event; // 记录 Note On
                event.tick = quantizedTick;                // 量化 Note On
            }
            else if (event.isNoteOff()) {
                int key = event.getKeyNumber();
                if (activeNotes.find(key) != activeNotes.end()) {
                    smf::MidiEvent* noteOn = activeNotes[key];
                    int duration = getDuration(*noteOn, event);
                    duration = quantizeTicks(duration, ppq, division); // 量化时值
                    setDuration(*noteOn, event, duration);             // 调整 Note Off 时间
                    activeNotes.erase(key);                           // 移除已完成的音符
                }
            }
        }
    }

    midiFile.sortTracks(); // 确保事件按时间排序
    midiFile.write(outputFile);
}

int main(int argc, char* argv[]) {
    string inputFile = "input.mid";
    string outputFile = "output.mid";
    int division = 8; // 量化到 16 分音符
    if (argc == 4)
    {
        inputFile = argv[1];
        outputFile = argv[2];
        division = atoi(argv[3]);
        quantizeMidi(inputFile, outputFile, division);
        cout << "MIDI 文件量化完成，保存到 " << outputFile << endl;
    }
    else
        cout << "argc missing\nMidQuantization input.mid output.mid (Musical note division:2 4 8 16Etc)\n";
    return 0;
}
