"""
midi_to_c_array.py
------------------
Convert a MIDI file thanh C array dang:
    const Tone_TypeDef melody[] = { {freq, duration_ms}, ... };

Cai thu vien truoc:
    pip install mido

Dung:
    python midi_to_c_array.py overworld.mid
    python midi_to_c_array.py overworld.mid --track 1 --output melody.h
    python midi_to_c_array.py overworld.mid --freq-scale 2.0 --dur-scale 0.1
"""

import mido
import argparse


# Helpers

def midi_note_to_freq(note: int) -> int:
    return round(440.0 * (2.0 ** ((note - 69) / 12.0)))


def ticks_to_ms(ticks: int, tempo: int, ticks_per_beat: int) -> int:
    return round((ticks / ticks_per_beat) * (tempo / 1000))


def extract_tempo(midi_file: mido.MidiFile) -> int:
    for track in midi_file.tracks:
        for msg in track:
            if msg.type == "set_tempo":
                return msg.tempo
    return 500000  # fallback: 120 BPM


def extract_notes(track: mido.MidiTrack, tempo: int, tpb: int):
    """
    Tai moi thoi diem chi giu 1 not cao nhat.
    Phu hop voi buzzer active chi phat duoc 1 am.
    """
    events = []
    current_time = 0
    for msg in track:
        current_time += msg.time
        if hasattr(msg, "channel") and msg.channel == 9:
            continue
        if msg.type == "note_on" and msg.velocity > 0:
            events.append((current_time, "on", msg.note))
        elif msg.type == "note_off" or (msg.type == "note_on" and msg.velocity == 0):
            events.append((current_time, "off", msg.note))

    notes = []
    active_notes: set = set()
    prev_tick = 0
    prev_note = None

    for tick, etype, note in events:
        if prev_note is not None and tick > prev_tick:
            duration_ms = ticks_to_ms(tick - prev_tick, tempo, tpb)
            if duration_ms > 0:
                notes.append((midi_note_to_freq(prev_note), duration_ms))

        if etype == "on":
            active_notes.add(note)
        else:
            active_notes.discard(note)

        prev_note = max(active_notes) if active_notes else None
        prev_tick = tick

    return notes


def transform_notes(notes: list, freq_scale: float = 1.0, dur_scale: float = 1.0):
    """
    Bien doi mang notes:
      - freq_scale: he so nhan tan so (2.0 = tang 1 octave)
      - dur_scale: he so nhan duration (0.1 = chia 10)
    """
    result = []
    for freq, dur in notes:
        new_freq = round(freq * freq_scale)
        new_dur = round(dur * dur_scale)
        # Bo qua not co duration = 0 sau khi scale
        if new_dur > 0:
            result.append((new_freq, new_dur))
    return result


def notes_to_c_array(notes: list, array_name: str = "melody") -> str:
    lines = [f"static const Tone_TypeDef {array_name}[] = {{"]
    for freq, dur in notes:
        lines.append(f"    {{{freq}, {dur}}},")
    lines.append("};")
    lines.append(f"\n// Total notes: {len(notes)}")
    return "\n".join(lines)


# Main

def main():
    parser = argparse.ArgumentParser(description="Convert MIDI to C Tone_TypeDef array")
    parser.add_argument("midi_file")
    parser.add_argument("--track", type=int, default=1)
    parser.add_argument("--name", default="melody")
    parser.add_argument("--output", default=None)
    parser.add_argument("--freq-scale", type=float, default=4.0,
                        help="He so nhan tan so (mac dinh: 4.0 = tang 1 octave)")
    parser.add_argument("--dur-scale", type=float, default=0.1,
                        help="He so nhan duration (mac dinh: 0.1 = chia 10)")
    args = parser.parse_args()

    mid = mido.MidiFile(args.midi_file)
    tempo = extract_tempo(mid)
    tpb = mid.ticks_per_beat

    print(f"File       : {args.midi_file}")
    print(f"Tracks     : {len(mid.tracks)}")
    print(f"Tempo      : {tempo} us/beat  ({round(60_000_000 / tempo)} BPM)")
    print(f"Ticks/beat : {tpb}")
    print(f"Freq scale : x{args.freq_scale}")
    print(f"Dur scale  : x{args.dur_scale}")

    print("\nDanh sach tracks:")
    for i, t in enumerate(mid.tracks):
        print(f"  [{i}] {t.name!r:30s}  ({len(t)} messages)")

    if args.track >= len(mid.tracks):
        print(f"\nLoi: track {args.track} khong ton tai!")
        return

    notes = extract_notes(mid.tracks[args.track], tempo, tpb)
    print(f"\nTrack [{args.track}]: {len(notes)} notes extracted (truoc transform)")

    # Apply transform
    notes = transform_notes(notes, args.freq_scale, args.dur_scale)
    print(f"After transform: {len(notes)} notes")

    c_code = notes_to_c_array(notes, args.name)

    if args.output:
        with open(args.output, "w") as f:
            f.write(c_code + "\n")
        print(f"Saved to: {args.output}")
    else:
        print("\n" + "-" * 60)
        print(c_code)


if __name__ == "__main__":
    main()