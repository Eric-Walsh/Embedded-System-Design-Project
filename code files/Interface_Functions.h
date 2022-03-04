#include <MKL25Z4.h>

void init_UI(void);

void Input(int switch_num);

void UpdateMenu(uint32_t MenuString[]);

void UpdateBPM(int beats);

void UpdateSampleRate(int rate);

void MoveCursor(int change);

void SelectFile(void);

void RateConverter(uint32_t *hexArray, int rate);

void init_Menu(void);
