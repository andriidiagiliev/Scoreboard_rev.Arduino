// работа с бегущим текстом

// **************** НАСТРОЙКИ ****************
#define TEXT_DIRECTION 1  // 1 - по горизонтали, 0 - по вертикали
#define MIRR_V 0          // отразить текст по вертикали (0 / 1)
#define MIRR_H 0          // отразить текст по горизонтали (0 / 1)

#define TEXT_HEIGHT 0     // высота, на которой бежит текст (от низа матрицы)
#define LET_WIDTH 5       // ширина буквы шрифта
#define LET_HEIGHT 8      // высота буквы шрифта
#define SPACE 1           // пробел

// --------------------- ДЛЯ РАЗРАБОТЧИКОВ ----------------------

int offset = WIDTH;

void fillString(String &text) {
  if (loadingFlag) {
    offset = WIDTH;   // перемотка в правый край
    loadingFlag = false;
    fullTextFlag = false;
  }

  if (millis() - scrollTimer >= D_TEXT_SPEED) {
    scrollTimer = millis();
    strip.clear();
    int i = 0, j = 0;
    while (text[i] != '\0') {
      if ((byte)text[i] > 191) {    // работаем с русскими буквами!
        i++;
      } else {
        drawLetter(j, text[i], offset + j * (LET_WIDTH + SPACE), setColor(j));
        i++;
        j++;
      }
    }
    fullTextFlag = false;

    offset--;
    if (offset < -j * (LET_WIDTH + SPACE)) {    // строка убежала
      offset = WIDTH + 3;
      fullTextFlag = true;
    }
    strip.show();
  }
}

//------------------------------ for static text ---------------------
void fillFrame(String &text) {
  
  int i = 0, j = 0;
  
    offset = (WIDTH - stringLength(text)*6)/2;   // перемотка в правый край
    loadingFlag = false;
    fullTextFlag = false;
  
    strip.clear();
  
    while (text[i] != '\0') {
      if ((byte)text[i] > 191) {    // работаем с русскими буквами!
        i++;
      } else {
        drawLetter(j, text[i], offset + j * (LET_WIDTH + SPACE), setColor(j));
        i++;
        j++;
      }
    }
  
    fullTextFlag = true;
  
    strip.show();

}



// -------------------------------------------------------------------


int stringLength(String &thisString) {
  int i = 0, j = 0;
  while (thisString[i] != '\0') {
    if ((byte)thisString[i] > 191) {    // работаем с русскими буквами!
      i++;
    } else {
      i++;
      j++;
    }
  }
  return j;
}

uint32_t setColor(int index) {
  for (byte i = 0; i < 10; i++) {
    if (index < colorChange[i]) {
      return pgm_read_dword(&textColors[i]);
    }
  }
}

void drawLetter(int index, uint8_t letter, int offset, uint32_t letterColor) {
  int start_pos = 0, finish_pos = LET_WIDTH;

  if (offset < -LET_WIDTH || offset > WIDTH) return;
  if (offset < 0) start_pos = -offset;
  if (offset > (WIDTH - LET_WIDTH)) finish_pos = WIDTH - offset;

  for (int i = start_pos; i < finish_pos; i++) {
    int thisByte;
    if (MIRR_V) thisByte = getFont((byte)letter, LET_WIDTH - 1 - i);
    else thisByte = getFont((byte)letter, i);

    for (byte j = 0; j < LET_HEIGHT; j++) {
      boolean thisBit;

      if (MIRR_H) thisBit = thisByte & (1 << j);
      else thisBit = thisByte & (1 << (LET_HEIGHT - 1 - j));

      // рисуем столбец (i - горизонтальная позиция, j - вертикальная)
      if (TEXT_DIRECTION) {
        if (thisBit) leds[getPixelNumber(offset + i, TEXT_HEIGHT + j)] = mHEX(letterColor);
        else drawPixelXY(offset + i, TEXT_HEIGHT + j, 0x000000);
      } else {
        if (thisBit) leds[getPixelNumber(i, offset + TEXT_HEIGHT + j)] = mHEX(letterColor);
        else drawPixelXY(i, offset + TEXT_HEIGHT + j, 0x000000);
      }

    }
  }
}

// ------------- СЛУЖЕБНЫЕ ФУНКЦИИ --------------

// интерпретатор кода символа в массиве fontHEX (для Arduino IDE 1.8.* и выше)
uint8_t getFont(uint8_t font, uint8_t row) {
  font = font - '0' + 16;   // перевод код символа из таблицы ASCII в номер согласно нумерации массива
  if (font <= 90) return pgm_read_byte(&(fontHEX[font][row]));     // для английских букв и символов
  else if (font >= 112 && font <= 159) {    // и пизд*ц ждя русских
    return pgm_read_byte(&(fontHEX[font - 17][row]));
  } else if (font >= 96 && font <= 111) {
    return pgm_read_byte(&(fontHEX[font + 47][row]));
  }
}

/*
  // интерпретатор кода символа по ASCII в его номер в массиве fontHEX (для Arduino IDE до 1.6.*)
  uint8_t getFontOld(uint8_t font, uint8_t row) {
  font = font - '0' + 16;   // перевод код символа из таблицы ASCII в номер согласно нумерации массива
  if (font < 126) return pgm_read_byte(&(fontHEX[font][row]));   // для английских букв и символов
  else return pgm_read_byte(&(fontHEX[font - 65][row]));         // для русских букв и символов (смещение -65 по массиву)
  }
*/