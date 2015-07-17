//
//  QRReaderView.m
//  NavigineDemo
//
//  Created by Администратор on 04/02/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//


#import "QRReaderView.h"


@implementation QRReaderView

-(IBAction)scanButtonTapped{
  // Создаем объект, получающий картинку с камеры
  ZBarReaderViewController *reader = [ZBarReaderViewController new];
  reader.readerDelegate = self;
  reader.supportedOrientationsMask = ZBarOrientationMaskAll;
  ZBarImageScanner *scanner = reader.scanner;
  /*
   Мы будем считывать только QR-коды. Для этого мы сперва отключим распознавание всех поддерживаемых библиотекой штрих-кодов…
   */
  [scanner setSymbology:0 config:ZBAR_CFG_ENABLE to:0];
  /*
   … а затем включим распознавание QR-кодов:
   */
  [scanner setSymbology:ZBAR_QRCODE config:ZBAR_CFG_ENABLE to:1];
  reader.readerView.zoom = 1.0;
  /*
   Далее показываем пользователю в модальном режиме представление (форму), сканирующую QR-код:
   */
  [self presentModalViewController:reader animated:YES];
}

- (BOOL) shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation) interfaceOrientation
{
  return(YES);
}

-(void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
  /*
   В результате распознавания  в info в общем случае возвращается что-то «перечисляемое»:
   */
  id<NSFastEnumeration> results = [info objectForKey:ZBarReaderControllerResults];
  /*
   Опять же в общем случае результатов может быть несколько, следующий код всего лишь берет первый считанный и распознанный QR-код:
   */
  ZBarSymbol *symbol = nil;
  
  for(symbol in results)
    // Считываем первый полученный результат
    break;
  /*
   Результат считывания и распознавания заносим в текстовое поле на форме через аутлет resultText:
   symbol.data;
*/
  /*
   Исходную картинку с QR-кодом заносим в объект UIImageView через аутлет resultImage:
   */
  self.resultImage.image = [info objectForKey: UIImagePickerControllerOriginalImage];
  
  // Прячем форму, с помощью которой фотографировали QR-код
  [picker dismissModalViewControllerAnimated:YES];
}


@end