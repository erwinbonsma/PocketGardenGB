#include "Sfx.h"

namespace Gamebuino_Meta {
// This code is automatically generated using import-from-p8.py
//
// It has been manually patched to:
// - Shorten SFX
// - Remove duplicate pattern

// Decay sound
const NoteSpec sfx0Notes[3] = {
    NoteSpec { .note=Note::C4, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::F3, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::Fs2, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
};
const TuneSpec sfx0 = TuneSpec {
    .noteDuration = 1, .loopStart = 3, .numNotes = 3, .notes = sfx0Notes
};

// Lively sound #1
const NoteSpec sfx1Notes[5] = {
    NoteSpec { .note=Note::Gs3, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::B3, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::Cs4, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::E4, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::As4, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
};
const TuneSpec sfx1 = TuneSpec {
    .noteDuration = 1, .loopStart = 5, .numNotes = 5, .notes = sfx1Notes
};

// Lively sound #2
const NoteSpec sfx2Notes[5] = {
    NoteSpec { .note=Note::E4, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::A4, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::C5, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::Ds5, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::E5, .vol=7, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
};
const TuneSpec sfx2 = TuneSpec {
    .noteDuration = 1, .loopStart = 5, .numNotes = 5, .notes = sfx2Notes
};

// Lively sound #3
const NoteSpec sfx3Notes[5] = {
    NoteSpec { .note=Note::F4, .vol=7, .wav=WaveForm::TILTED_SAW, .fx=Effect::NONE },
    NoteSpec { .note=Note::A4, .vol=7, .wav=WaveForm::TILTED_SAW, .fx=Effect::NONE },
    NoteSpec { .note=Note::B4, .vol=7, .wav=WaveForm::TILTED_SAW, .fx=Effect::NONE },
    NoteSpec { .note=Note::Ds5, .vol=7, .wav=WaveForm::TILTED_SAW, .fx=Effect::NONE },
    NoteSpec { .note=Note::F5, .vol=7, .wav=WaveForm::TILTED_SAW, .fx=Effect::NONE },
};
const TuneSpec sfx3 = TuneSpec {
    .noteDuration = 1, .loopStart = 5, .numNotes = 5, .notes = sfx3Notes
};

// Lively sound #4
const NoteSpec sfx4Notes[5] = {
    NoteSpec { .note=Note::Cs4, .vol=7, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::F4, .vol=7, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::As4, .vol=7, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::Cs5, .vol=7, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::Cs5, .vol=7, .wav=WaveForm::PHASER, .fx=Effect::NONE },
};
const TuneSpec sfx4 = TuneSpec {
    .noteDuration = 1, .loopStart = 5, .numNotes = 5, .notes = sfx4Notes
};

// Game Over
const NoteSpec sfx5Notes[8] = {
    NoteSpec { .note=Note::A4, .vol=7, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::G4, .vol=6, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::E4, .vol=5, .wav=WaveForm::PHASER, .fx=Effect::NONE },
    NoteSpec { .note=Note::E4, .vol=4, .wav=WaveForm::PHASER, .fx=Effect::SLIDE },
    NoteSpec { .note=Note::E4, .vol=3, .wav=WaveForm::PHASER, .fx=Effect::SLIDE },
    NoteSpec { .note=Note::E4, .vol=2, .wav=WaveForm::PHASER, .fx=Effect::SLIDE },
    SILENCE,
    SILENCE,
};
const TuneSpec sfx5 = TuneSpec {
    .noteDuration = 24, .loopStart = 8, .numNotes = 8, .notes = sfx5Notes
};

// Level-hi
const NoteSpec sfx6Notes[6] = {
    NoteSpec { .note=Note::F4, .vol=4, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::A4, .vol=5, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::C5, .vol=6, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
    NoteSpec { .note=Note::C5, .vol=6, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
    NoteSpec { .note=Note::C5, .vol=5, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
    NoteSpec { .note=Note::C5, .vol=4, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
};
const TuneSpec sfx6 = TuneSpec {
    .noteDuration = 20, .loopStart = 6, .numNotes = 6, .notes = sfx6Notes
};

const NoteSpec sfx7Notes[6] = {
    NoteSpec { .note=Note::F4, .vol=4, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::E4, .vol=5, .wav=WaveForm::TRIANGLE, .fx=Effect::NONE },
    NoteSpec { .note=Note::C4, .vol=6, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
    NoteSpec { .note=Note::C4, .vol=6, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
    NoteSpec { .note=Note::C4, .vol=5, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
    NoteSpec { .note=Note::C4, .vol=4, .wav=WaveForm::TRIANGLE, .fx=Effect::VIBRATO },
};
const TuneSpec sfx7 = TuneSpec {
    .noteDuration = 20, .loopStart = 6, .numNotes = 6, .notes = sfx7Notes
};

const TuneSpec *const pattern0Tunes[1] = { &sfx5 };
const PatternSpec pattern0 = PatternSpec {
    .numTunes = 1, .tunes = pattern0Tunes
};
const TuneSpec *const pattern1Tunes[1] = { &sfx6 };
const PatternSpec pattern1 = PatternSpec {
    .numTunes = 1, .tunes = pattern1Tunes
};
const TuneSpec *const pattern3Tunes[1] = { &sfx7 };
const PatternSpec pattern3 = PatternSpec {
    .numTunes = 1, .tunes = pattern3Tunes
};

// Game Over + Level Hi
const PatternSpec *const song0Patterns[2] = {
    &pattern0,
    &pattern1,
};
const SongSpec song0 = SongSpec {
    .loopStart = 1, .numPatterns = 2, .patterns = song0Patterns
};

// Game Over + Level Lo
const PatternSpec *const song2Patterns[2] = {
    &pattern0,
    &pattern3,
};
const SongSpec song2 = SongSpec {
    .loopStart = 1, .numPatterns = 2, .patterns = song2Patterns
};


}; // Namespace

const Gamebuino_Meta::TuneSpec* decaySfx = &Gamebuino_Meta::sfx0;

const Gamebuino_Meta::TuneSpec* aliveSfx[4] = {
  &Gamebuino_Meta::sfx1, &Gamebuino_Meta::sfx2, &Gamebuino_Meta::sfx3, &Gamebuino_Meta::sfx4
};

const Gamebuino_Meta::TuneSpec* gameOverSfx = &Gamebuino_Meta::sfx5;

const Gamebuino_Meta::SongSpec* levelHiSong = &Gamebuino_Meta::song0;
const Gamebuino_Meta::SongSpec* levelLoSong = &Gamebuino_Meta::song2;
