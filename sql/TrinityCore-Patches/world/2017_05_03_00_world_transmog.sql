SET @TEXT_ID := 65000;
REPLACE INTO `npc_text` (`ID`, `text0_0`) VALUES
(@TEXT_ID, 'Трансмогрификация позволяет вам изменять внешний вид ваших предметов без изменения их характеристик.\r\ Предметы, использованные при трансмогрификации, больше не подлежат возврату, обмену и привязаны к вам.\r\N Обновление меню обновляет вид и цены.\r\n\r\N Не все может быть преобразовано друг в друга.\r\N Ограничения включают, но не ограничиваются ими:\r\ Не только броня и оружие могут быть трансмогрифицированы\r\ nGuns, луки и арбалеты могут быть трансмогрифицированы друг с другом \ r \ n Рыболовные шесты не могут быть трансмогрифицированы \ r \ NY Вы должны быть в состоянии оснастить оба предмета, используемые в процессе.\r\n\r\Птрансмогрификации остаются на ваших товарах до тех пор, пока они принадлежат вам.\r\ Если вы попытаетесь положить предмет в банк гильдии или отправить его по почте кому-то другому, трансмогрификация будет отменена.\r\n\r\ny Вы также можете бесплатно удалить трансмогрификации в трансмогрифиторе.'),
(@TEXT_ID+1, 'Вы можете сохранить свои собственные наборы для трансмогрификации.\r\n \r\n Чтобы сохранить, сначала вы должны трансмогрифицировать свои экипированные предметы.\r \n Затем, когда вы заходите в меню управления настройками и переходите к меню сохранения настроек, отображаются все элементы, которые вы преобразовали, чтобы вы могли видеть, что вы сохраняете.\ r\ Если вы считаете, что набор подходит, вы можете нажать, чтобы сохранить набор, и назвать его по своему усмотрению.\r\n\r\n Чтобы использовать набор, вы можете щелкнуть сохраненный набор в меню управления набором, а затем выбрать использовать набор.\r\Если в наборе есть трансмогрификация для предмета, который уже трансмогрифицирован, старая трансмогрификация утрачена.\r\N Обратите внимание, что при попытке использовать набор применяются те же ограничения на трансмогрификацию, что и при обычной трансмогрификации.\r\n\r\n Чтобы удалить набор, вы можете перейти в меню набора и выбрать удалить набор.');

SET @STRING_ENTRY := 11100;
REPLACE INTO `trinity_string` (`entry`, `content_default`) VALUES
(@STRING_ENTRY+0, 'Элемент преобразован'),
(@STRING_ENTRY+1, 'Слот оборудования пуст'),
(@STRING_ENTRY+2, 'Выбран недопустимый исходный элемент'),
(@STRING_ENTRY+3, 'Исходный элемент не существует'),
(@STRING_ENTRY+4, 'Элемент назначения не существует'),
(@STRING_ENTRY+5, 'Выбранные элементы недействительны'),
(@STRING_ENTRY+6, 'Недостаточно денег'),
(@STRING_ENTRY+7, 'У вас недостаточно токенов'),
(@STRING_ENTRY+8, 'Трансмогрификации удалены'),
(@STRING_ENTRY+9, 'Трансмогрификаций нет'),
(@STRING_ENTRY+10, 'Вставлено недопустимое имя');