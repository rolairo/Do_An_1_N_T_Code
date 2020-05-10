#include <16f877.h>
#use I2C(MASTER,SDA = PIN_C4, SCL = PIN_C3)
#INCLUDE <stdlibm.h>
#include <string.h> 
#FUSES NOWDT, HS, PUT, NOPROTECT , NOLVP
#use delay(CLOCK=20MHz) 
#include "LCD2004.c"
#include "DS1307.c"
unsigned char mangKyTu [] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
unsigned char key;
unsigned char tk[8];
unsigned char mk[8];
unsigned INT1 isDangNhapSuccess = 0;
unsigned char *tkAdmin = "123";
unsigned INT1 isAdmin =0;
unsigned INT8 vitriTK =0;
unsigned INT8 vitriMK =0;
unsigned INT8 vitriTKSelected;
unsigned INT1 TKMKisInPuted=0;
unsigned INT1 isTaoTK =0;
unsigned INT8 isLevelMhOfAdmin =0;
unsigned INT8 isLevelMhOfUser =0;
unsigned INT1 isInputOK = 0;
unsigned INT8 soLanSai = 0;
unsigned char *arr[15];
unsigned INT8 tang = 0;
unsigned INT8 tangcu = 0;
unsigned INT8 giam = 0;
unsigned INT8 numBerOfMangContro = 0;
unsigned INT1 userDaDangNhap = 0;
unsigned char lichSuLogin[8];

UNSIGNED char KEY_NHAN()
{
   SIGNED INT8 MAQUETCOT, MAPHIM, HANG, COT;
   MAQUETCOT = 0B11111110; MAPHIM = HANG = 0XFF;
   for (COT = 0; COT < 4; COT++)
   {
      OUTPUT_B (MAQUETCOT); MAQUETCOT = (MAQUETCOT<<1) + 0x1;
      if      (!INPUT (PIN_B4))  {HANG = 0; BREAK; }
      else if (!INPUT (PIN_B5))  {HANG = 1; BREAK; }
      else if (!INPUT (PIN_B6))  {HANG = 2; BREAK; }
      else if (!INPUT (PIN_B7))  {HANG = 3; BREAK; }
   }

   if (HANG != 0XFF) MAPHIM = mangKyTu[COT * 4 + HANG];
   RETURN (MAPHIM);
}

void writeEEPROM(UNSIGNED char arr[], unsigned int8 vitri)
{
   INT i;
   for(i = 0; arr[i] != '\0'; i++)
   {
      write_eeprom(vitri + i, arr[i]);
   }
   for (; i < 8; i++)
   {
      write_eeprom (vitri + i, 0xff);
   }

}

void readEEPROM(UNSIGNED char arr[], unsigned int8 vitri)
{
   UNSIGNED int8 i;
   char c;
   for(i = 0; i < 8; i++)
   {
      c = read_eeprom(vitri + i) ;
      if (c == 0xff)
      {
         arr[i] = '\0';
         RETURN;
      }

      else
      arr[i] = c;
   }
}

void manHinhThongBaoSai()
{
   lcd_clear();
   lcd_GOTO_xy(1, 0);
   lcd_data(" Thong tin TK sai ! ");
   delay_ms(3000);
}

void inputData(UNSIGNED char arr[], unsigned int8 vitri)
{
   arr[vitri] = key;
}

void DisplayInputTK(UNSIGNED char arr[], unsigned int8 hang)
{
   lcd_GOTO_xy(hang, vitriTK + 3);
   lcd_data(key);
   inputData(arr, vitriTK);
}

void DisplayInputMK(UNSIGNED char arr[])
{
   lcd_GOTO_xy(1, vitriMK + 3);
   if (key == ' ')
      lcd_data (' ');
   else
      lcd_data('*');
   inputData (arr, vitriMK);
}

void mHAdmin()
{
   lcd_clear();
   lcd_GOTO_xy (0, 0);
   lcd_data("1.Them TK   2.Xoa TK");
   lcd_GOTO_xy (1, 0);
   lcd_data("3.Doi Mk    4.Exit");
   lcd_GOTO_xy (2, 0);
   lcd_data("5.Lich su Dang Nhap");
   lcd_GOTO_xy(3, 0);
   lcd_data ("Nhap : ");
}

void mHUser()
{
   lcd_clear() ;
   lcd_GOTO_xy(1, 0);
   lcd_data("1.Doi mat khau");
   lcd_GOTO_xy(2, 0);
   lcd_data("2.Thoat");
   lcd_GOTO_xy(3, 0);
   lcd_data("Nhap:");
}

void resetTKMK()
{
   UNSIGNED int8 i;
   vitriTK = 0;
   vitriMK = 0;
   for (i = 0; i < 8; i++)
   {
      tk[i] = 0;
      mk[i] = 0;
   }
}

unsigned INT1 checkMK()
{
   
   UNSIGNED char arr[8];
   mk[vitriMK] = '\0';
   isAdmin = 0;
   readEEPROM(arr, vitriTKSelected + 8);
   if (strcmp(arr, mk) ==  0)
   {
      if (strcmp(tkAdmin, tk) == 0)
      {
         isAdmin = 1;
         mHAdmin();
      }

      else
      {
         userDaDangNhap = 1;
         DOC_THOI_GIAN_TU_REALTIME();
         lichSuLogin[0] = GIO_DS13 + 1;
         lichSuLogin[1] = PHUT_DS13 + 1;
         lichSuLogin[2] = GIAY_DS13 + 1;
         lichSuLogin[3] = NGAY_DS13 + 1;
         lichSuLogin[4] = THANG_DS13 + 1;
         lichSuLogin[5] = NAM_DS13 + 1;
         lichSuLogin[6] = 0;
         writeEEPROM(lichSuLogin, 248);
         writeEEPROM(tk, 240);
         mHUser();
      }

      
      resetTKMK();
      RETURN 1;
   }

   RETURN 0;
}

int8 checkTK()
{
   
   UNSIGNED char arr[8];
   UNSIGNED int16 i;
   tk[vitriTK] = '\0';
   for (i = 0; i <= 224; i += 16)
   {
      if (read_eeprom (i) == 0xff)
      CONTINUE;
      readEEPROM(arr, i);
      if (strcmp(arr, tk) ==  0)
      {
         vitriTKSelected = i;
         RETURN 1;
      }
   }

   RETURN 0;
}

void manHinhDangNhap()
{
   lcd_clear();
   lcd_GOTO_xy(0, 0);
   lcd_data("TK:");
   lcd_GOTO_xy(1, 0);
   lcd_data ("MK:") ;
   lcd_GOTO_xy(2, 0);
   lcd_data(" DO dai max = 8");
   lcd_GOTO_xy(3, 0);
   lcd_data(" Nhan D de xac nhan");
   lcd_GOTO_xy(0, 3);
}

void xulyDangNhapSai()
{
   manHinhThongBaoSai();
   manHinhDangNhap();
}

unsigned INT1 kiemTraTKMK()
{
   if (checkTK() == 0 || checkMK() == 0)
      RETURN 0;
   RETURN 1;
}

void manHinhThemUser()
{
   resetTKMK();
   lcd_clear();
   lcd_GOTO_xy(0, 0); lcd_data("TK:"); lcd_goto_xy(1, 0); lcd_data("MK:") ;
   lcd_GOTO_xy(2, 0); lcd_data(" Tao User"); lcd_goto_xy(0, 3);
}

void manHinhXoaTaiKhoan()
{
   
   resetTKMK();
   lcd_clear();
   lcd_GOTO_xy(3, 0);
   lcd_data("TK:");
}

void manHinhDoiMatKhau()
{
   lcd_clear();
   lcd_GOTO_xy(0, 0); lcd_data(" Nhap MK moi:");
   lcd_GOTO_xy(1, 0); lcd_data("MK:");
}

void thongBaoDoiMKTC()
{
   lcd_clear();
   lcd_GOTO_xy(1, 0); lcd_data(" Doi MK thanh cong ! "); delay_ms(2000);
}

void xuLyInputTK(UNSIGNED int8 hang)
{
   if(TKMKisInPuted == 0)
   {
      if(key != 'A' && key != 'B' && key != 'C')
      {
         if(key != 'D' && key != '#')
         {
            if(vitriTK < 8)
            {
               DisplayInputTK(tk, hang);
               vitriTK++;
            }
         }

         else if(key == '#')
         {
            if(vitriTK > 0)
            {
               vitriTK--;
               key = ' ';
               DisplayInputTK (tk, hang) ;
               lcd_GOTO_xy(hang, vitriTK + 3);
            }

         }

         else
         {
            TKMKisInPuted = 1;
            key = 0xff;
            lcd_GOTO_xy(1, 3);
         }
      }
   }
}

void xulyInputMK()
{
   if (TKMKisInPuted == 1)
   {
      if (key != 'A' && key != 'B' && key != 'C')
      {
         if (key != 'D' && key != 0xff && key != '#')
         {
            if (vitriMK < 8)
            {
               DisplayInputMK (mk) ;
               vitriMK++;
            }
         }

         else if (key == '#')
         {
            if (vitriMK > 0)
            {
               vitriMK--;
               key = ' ';
               DisplayInputMK(mk);
               lcd_GOTO_xy(1, vitriMK + 3);
            }

         }

         else if (key == 'D')
         {
            isInputOK = 1;
         }
      }
   }
}

void thietLapAdmin1()
{
   isLevelMhOfAdmin = 1; resetTKMK(); manHinhThemUser(); TKMKisInPuted =  0;
}

void thietLapAdmin2()
{
   isLevelMhOfAdmin = 2; TKMKisInPuted = 0; resetTKMK(); manHinhXoaTaiKhoan();
}

void thietLapAdmin3()
{
   resetTKMK();
   isLevelMhOfAdmin = 3;
   TKMKisInPuted = 1;
   lcd_clear(); lcd_GOTO_xy(0, 0); lcd_data(" Nhap MK moi:");
   lcd_GOTO_xy(1, 0);
   lcd_data("MK:");
}

void backAdmin()
{
   resetTKMK();
   isLevelMhOfAdmin = 0;
   TKMKisInPuted = 0;
   isTaoTK = 0;
   mHAdmin();
}

void readAllEpprom()
{
   UNSIGNED int16 i;
   for (i = 16; i <= 224;i = i + 16)
   {
      if (read_eeprom(i) != 0xff)
      {
         char * d = malloc(8);
         readEEPROM(d, i) ;
         arr[numberOfMangContro] = d;
         numberOfMangContro++;
      }

   }
}

void hienthi(UNSIGNED int8 vitri)
{
   UNSIGNED int8 i = 0;
   while (*(arr[vitri] + i) !=  0)
   {
      lcd_data (*(arr[vitri] + i));
      i++;
   }
}

void hienThiNDungCtRaLCD(UNSIGNED int8 i)
{
   UNSIGNED int8 chuc;
   UNSIGNED int8 donvi;
   chuc = (i + 1) / 10;
   donvi = (i + 1) % 10;
   lcd_GOTO_xy(i % 3, 0);
   if (chuc == 0)
   lcd_data(donvi + 0x30) ;
   else
   {
      lcd_data (chuc + 0x30) ;
      lcd_data (donvi + 0x30) ;
   }

   lcd_data(".");
   hienthi(i);
   lcd_GOTO_xy(3, 0);
   lcd_data ("TK:");
}

void xuLyXemThem()
{
   UNSIGNED int8 i;
   if (numberOfMangContro == 0)
   {
      lcd_GOTO_xy(0, 0);
      lcd_data(" Khong co User nao ! ");
   }

   else if(tang < numberOfMangContro)
   {
      lcd_clear();
      tangcu = tang;
      if (numberOfMangContro < tang + 3)
         tang += (numberOfMangContro % 3);
      else
         tang += 3;
      for (i = tangcu; i < tang; i++)
         hienThiNDungCtRaLCD(i);
      giam = tangcu;
   }
}

void main()
{
   UNSIGNED int16 i;
   UNSIGNED char c;
   // UNSIGNED char arr2[8];
   UNSIGNED char arrXoa[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   UNSIGNED int1 resultTaoTK;
 /*  THIET_LAP_THOI_GIAN_HIEN_TAI () ;
   NAP_THOI_GIAN_HTAI_VAO_DS13B07 () ;*/
   lcd_setup();
   lcd_clear();
   set_tris_d(0x00);
   set_tris_e(0x00);
   set_tris_b(0b11110000);
   port_b_pullups(0b00001111);
   lcd_setup();
   lcd_clear();
   manHinhDangNhap();
   UNSIGNED char arr1[] = {'1', '2', '3', '\0'};
   writeEEPROM(arr1, 0); writeEEPROM(arr1, 8);
   while (true)
   {
      key = KEY_NHAN(); // quet phim
      if (key != 0xff)
      {
         delay_ms(50); // cho phim on dinh
         key = KEY_NHAN(); // kiem tra phim lai lan nua
         if (key != 0xff)
         {
            // Xu ly O man hinh dang nhap
            if (isDangNhapSuccess == 0)
            {
               xuLyInputTK(0);
               xulyInputMK();
               if (isInputOK == 1)
               {
                  if (kiemTraTKMK())
                  {
                     isDangNhapSuccess = 1; soLanSai = 0;
                  }

                  else
                  { xulyDangNhapSai(); soLanSai++; }
                  resetTKMK();
                  TKMKisInPuted = 0;
                  isInputOK = 0;
               }

               if (soLanSai == 3)
               {
                  lcd_clear();
                  lcd_GOTO_xy(1, 0);
                  lcd_data(" Bi khoa trong (s)");
                  output_high(pin_c0);
                  for (i = 5; i > 0; i--)
                  {
                     sl(i, 2, 8);
                     delay_ms(1000);
                  }

                  output_low(pin_c0); //bat loa
                  lcd_clear();
                  manHinhDangNhap();
                  soLanSai = 0;
               }
            }

            else
            {
               if (isAdmin)
               {
                  if (isLevelMhOfAdmin == 0)
                  {
                     if (key == '1')
                        thietLapAdmin1 () ;
                     else if (key == '2')
                     {
                        thietLapAdmin2(); 
                        tang = 0;numberOfMangContro = 0; 
                        readAllEpprom(); xuLyXemThem(); 
                     }
                     else if (key == '3')
                        thietLapAdmin3();
                     else if (key == '4')
                     {
                        resetTKMK();
                        isLevelMhOfAdmin = 0;
                        TKMKisInPuted = 0; isDangNhapSuccess = 0;
                        manHinhDangNhap();
                     }

                     else if (key == '5')
                     {
                        isLevelMhOfAdmin = 5;
                        lcd_clear();
                        lcd_GOTO_xy(0, 0);
                        lcd_data("Thong Tin Dang Nhap");
                        if (userDaDangNhap == 1)
                        {
                           readEEPROM(tk, 0xF0);
                           readEEPROM(lichSuLogin, 0xF8);
                           lcd_GOTO_Xy(1, 0);
                           lcd_data("User:");
                           i = 0;
                           while (tk[i] != 0) lcd_data(tk[i++]);
                           lcd_GOTO_xy (2, 4) ;
                           LCD_DATA ((lichSuLogin[0] - 1) / 16 + 0X30) ; 
                           LCD_DATA((lichSuLogin[0] - 1) % 16  + 0X30);
                                                      LCD_DATA (' ') ;
                           LCD_DATA ((lichSuLogin[1] - 1) / 16 + 0X30); 
                           LCD_DATA ((lichSuLogin[1] - 1) % 16  + 0X30);
                                                      LCD_DATA (' ') ;
                           LCD_DATA ((lichSuLogin[2] - 1) / 16 + 0X30); 
                           LCD_DATA ((lichSuLogin[2] - 1) % 16  + 0X30);
                                                      lcd_GOTO_xy (3, 4);
                           LCD_DATA ((lichSuLogin[3] - 1) / 16 + 0X30); 
                           LCD_DATA ((lichSuLogin[3] - 1) % 16  + 0X30);
                                                      LCD_DATA (' ') ;
                           LCD_DATA ((lichSuLogin[4] - 1) / 16 + 0X30);
                           LCD_DATA((lichSuLogin[4] - 1) % 16  + 0X30);
                                                      LCD_DATA (' ') ;
                                                      lcd_data ("20") ;
                           LCD_DATA ((lichSuLogin[5] - 1) / 16 + 0X30); 
                           LCD_DATA ((lichSuLogin[5] - 1) % 16 + 0X30);
                        }

                        else if(userDaDangNhap == 0)
                        {
                           lcd_GOTO_xy(1, 0);
                           lcd_data("Chua co US Dang Nhap");
                        }
                     }
                  }

                  else if (isLevelMhOfAdmin == 1)
                  {
                     
                     if (isTaoTK == 0)
                     {
                        if (key == 'D')
                        {
                           resultTaoTK = checkTK();
                           if (resultTaoTK == 1)
                           {
                              lcd_clear(); lcd_GOTO_xy (1, 0); 
                              lcd_data (" TK da ton tai ! ");
                              delay_ms (2000);
                              resetTKMK();
                              manHinhThemUser();
                           }

                           else
                           {
                              isTaoTK = 1;
                              TKMKisInPuted = 1;
                              lcd_GOTO_xy(1, 3);
                           }
                        }

                        else if (key == 'C')
                           backAdmin();
                        else xuLyInputTK(0);
                     }

                     else
                     {
                        if (key == 'D')
                        {
                           if (vitriMK > 0)
                           {
                              // TIm vung nho de luu tai khoa vao
                              for (i = 16; i <= 224; i += 16)
                              {
                                 c = read_eeprom(i);
                                 if (c == 0xff)
                                 BREAK;
                              }

                              if (i <= 224)
                              {
                                 writeEEPROM(tk, i);
                                 writeEEPROM(mk, i + 8);
                                 lcd_clear(); lcd_GOTO_xy(1, 0);
                                 lcd_data(" Tao Tk thanh cong ! ");
                                 delay_ms(2000);
                                 // reset toan bo bien de tro ve man hinh admin
                                 isLevelMhOfAdmin = 0;
                                 isTaoTK = 0;
                                 mHAdmin();
                              }

                              else
                              {
                                  lcd_clear(); lcd_GOTO_xy(1, 0); 
                                  lcd_data(" Het Dung luong"); 
                                  delay_ms(1000); mHAdmin();
                              }
                           }
                        }

                        else if (key == 'C')
                           backAdmin();
                        else
                           xulyInputMK();
                     }

                  }

                  else if (isLevelMhOfAdmin == 2)
                  {
                     if (key == 'D')
                     {
                        // ko cho xoa tk admin
                        if (checkTK() && strcmp(tk, tkAdmin))
                        {
                           writeEEPROM(arrXoa, vitriTKSelected);
                           writeEEPROM(arrXoa, vitriTKSelected + 8);
                           lcd_clear();
                           lcd_GOTO_xy(1, 0); lcd_data(" Xoa thanh cong ! ");
                        }

                        else
                        {
                           lcd_clear();
                           lcd_GOTO_xy(1, 0); lcd_data("Tai Khoan ko ton tai");
                        }

                        
                        isLevelMhOfAdmin = 0;
                        delay_ms(2000);
                        mHAdmin();
                     }

                     else if (key == 'C')
                     { backAdmin(); for (i = 0; i < 10; i++) free(arr[i]); }
                     else if (key == 'B')
                        xuLyXemThem();
                     else if (key == 'A')
                     {
                        if (giam > 0)
                        {
                           lcd_clear();
                           for (i = giam - 3; i < giam ; i++)
                              hienThiNDungCtRaLCD(i);
                           tang = giam;
                           giam = giam - 3;
                        }
                     }

                     else
                        xulyInputTK (3) ;
                  }

                  else if (isLevelMhOfAdmin == 3)
                  {
                     if (key == 'D')
                     {
                        mk[vitriMK] = '\0';
                        writeEEPROM(mk, 8);
                        thongBaoDoiMKTC();
                        isLevelMhOfAdmin = 0;
                        mHAdmin();
                     }

                     else if (key == 'C')
                        backAdmin();
                     else
                        xulyInputMK();
                  }
                  
                  else if (isLevelMhOfAdmin == 5)
                  {                                        
                     if (key == 'C')
                     {
                        backAdmin();
                     }
                  }
               }

               else
               {
                  if (isLevelMhOfUser == 0)
                  {
                     if (key == '1')
                     {
                        resetTKMK();
                        manHinhDoiMatKhau();
                        TKMKisInPuted = 1;
                        isLevelMhOfUser = 1;
                     }

                     else if (key == '2')
                     {
                        resetTKMK();
                        isLevelMhOfUser = 0;
                        TKMKisInPuted = 0; isDangNhapSuccess = 0;
                        manHinhDangNhap();
                     }
                  }

                  else if (isLevelMhOfUser == 1)
                  {
                     if (key == 'D')
                     {
                        mk[vitriMK] = '\0';
                        writeEEPROM(mk, vitriTKSelected + 8);
                        thongBaoDoiMKTC();
                        mHUser();
                        isLevelMhOfUser = 0;
                     }

                     else if (key == 'C')
                     {
                        resetTKMK();
                        isLevelMhOfUser = 0;
                        mHUser();
                     }

                     else
                     {
                        xulyInputMK();
                     }
                  }
               }
            }

            while (KEY_NHAN () != 0xff); // cho buong phim
            delay_ms(50); // cho phim on dinh
         }
      }
   } 
}

