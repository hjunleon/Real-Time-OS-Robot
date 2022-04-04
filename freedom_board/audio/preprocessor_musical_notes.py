import math

notes = []

for i in range(9):
    notes.append({})



notes[0]["A"] = 27.500000000000000
notes[0]["A#"] = 29.135235094880619
notes[0]["B"] = 30.867706328507756

notes[1]["C"] = 32.703195662574829
notes[1]["C#"] = 34.647828872109012
notes[1]["D"] = 36.708095989675945
notes[1]["D#"] = 38.890872965260113
notes[1]["E"] = 41.203444614108741
notes[1]["F"] = 43.653528929125485
notes[1]["F#"] = 46.249302838954299
notes[1]["G"] = 48.999429497718661
notes[1]["G#"] = 51.913087197493142
notes[1]["A"] = 55.000000000000000
notes[1]["A#"] = 58.270470189761239
notes[1]["B"] = 61.735412657015513

notes[2]["C"] = 65.406391325149658
notes[2]["C#"] = 69.295657744218024
notes[2]["D"] = 73.416191979351890
notes[2]["D#"] = 77.781745930520227
notes[2]["E"] = 82.406889228217482
notes[2]["F"] = 87.307057858250971
notes[2]["F#"] = 92.498605677908599
notes[2]["G"] = 97.998858995437323
notes[2]["G#"] = 103.826174394986284
notes[2]["A"] = 110.000000000000000
notes[2]["A#"] = 116.540940379522479
notes[2]["B"] = 123.470825314031027

notes[3]["C"] = 130.812782650299317
notes[3]["C#"] = 138.591315488436048
notes[3]["D"] = 146.832383958703780
notes[3]["D#"] = 155.563491861040455
notes[3]["E"] = 164.813778456434964
notes[3]["F"] = 174.614115716501942
notes[3]["F#"] = 184.997211355817199
notes[3]["G"] = 195.997717990874647
notes[3]["G#"] = 207.652348789972569
notes[3]["A"] = 220.000000000000000
notes[3]["A#"] = 233.081880759044958
notes[3]["B"] = 246.941650628062055

notes[4]["C"] = 261.625565300598634
notes[4]["C#"] = 277.182630976872096
notes[4]["D"] = 293.664767917407560
notes[4]["D#"] = 311.126983722080910
notes[4]["E"] = 329.627556912869929
notes[4]["F"] = 349.228231433003884
notes[4]["F#"] = 369.994422711634398
notes[4]["G"] = 391.995435981749294
notes[4]["G#"] = 415.304697579945138
notes[4]["A"] = 440.000000000000000
notes[4]["A#"] = 466.163761518089916;
notes[4]["B"] = 493.883301256124111

notes[5]["C"] = 523.251130601197269
notes[5]["C#"] = 554.365261953744192
notes[5]["D"] = 587.329535834815120
notes[5]["D#"] = 622.253967444161821
notes[5]["E"] = 659.255113825739859
notes[5]["F"] = 698.456462866007768
notes[5]["F#"] = 739.988845423268797
notes[5]["G"] = 783.990871963498588
notes[5]["G#"] = 830.609395159890277
notes[5]["A"] = 880.000000000000000
notes[5]["A#"] = 932.327523036179832
notes[5]["B"] = 987.766602512248223

notes[6]["C"] = 1046.502261202394538
notes[6]["C#"] = 1108.730523907488384
notes[6]["D"] = 1174.659071669630241
notes[6]["D#"] = 1244.507934888323642
notes[6]["E"] = 1318.510227651479718
notes[6]["F"] = 1396.912925732015537
notes[6]["F#"] = 1479.977690846537595
notes[6]["G"] = 1567.981743926997176
notes[6]["G#"] = 1661.218790319780554
notes[6]["A"] = 1760.000000000000000
notes[6]["A#"] = 1864.655046072359665
notes[6]["B"] = 1975.533205024496447

notes[7]["C"] = 2093.004522404789077
notes[7]["C#"] = 2217.461047814976769
notes[7]["D"] = 2349.318143339260482
notes[7]["D#"] = 2489.015869776647285
notes[7]["E"] = 2637.020455302959437
notes[7]["F"] = 2793.82585146403107
notes[7]["F#"] = 2959.955381693075191
notes[7]["G"] = 3135.963487853994352
notes[7]["G#"] = 3322.437580639561108
notes[7]["A"] = 3520.000000000000000
notes[7]["A#"] = 3729.310092144719331
notes[7]["B"] = 3951.066410048992894

notes[8]["C"] = 4186.009044809578154


finalStr = "#ifndef MUSIC_NOTES\n\
#define MUSIC_NOTES\n"

for i in range(9):
    keys = notes[i].keys()
    for k in keys:
        finalVal = (1/notes[i][k]) / (1/375000)
        finalStr += "#define {}{} {}\n".format(k.replace("#","s"),i, finalVal)
        # if finalVal.contains("#"):
        #     finalStr += "#define {}{} {}\n".format(k.replace("#","s"),i, finalVal)

finalStr += "#endif"
f = open("music_notes.c", "w")
f.write(finalStr)
f.close()
