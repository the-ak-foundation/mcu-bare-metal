#include <stdio.h>
#include "notes.h"

// Chia duration từ sequencer về đơn vị Tone_TypeDef (~10ms)
// Sequencer 120 BPM → 125ms/step → DURATION_DIV 10 → duration ≈ 12
// Sequencer 120 BPM → 125ms/step → DURATION_DIV 50 → duration ≈ 2 (shoot sound)
#define DURATION_DIV 10

int main() {
int melody[]        = { NOTE_B4, NOTE_A4, NOTE_G4, NOTE_F4 };
int noteDurations[] = {      25,      25,      25,      25 };
    int len = sizeof(melody) / sizeof(melody[0]);

    printf("static const Tone_TypeDef tones_shoot[] = {\n");
    for (int i = 0; i < len; i++) {
        int dur = noteDurations[i] / DURATION_DIV;
        if (dur < 1) dur = 1;
        printf("\t{%4d,%2d},\n", melody[i], dur);
    }
    printf("\t{   0, 0}\n");
    printf("};\n");

    return 0;
}
