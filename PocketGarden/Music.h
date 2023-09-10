#pragma once

// Starts music. It only needs to be invoked once.
void startMusic();

// Reset music at the start of a new game.
// It ensures the update happens as quickly as possible.
void resetMusic();

// Indicates if an update is due.
// Update of the music should be done as late as possible (to ensure
// it best reflects the active garden state), but not too late (to
// ensure it happens before the next pattern is played).
bool isMusicUpdateDue();

// Sets the next pattern to be played, based on the garden's current state.
void updateMusic();
