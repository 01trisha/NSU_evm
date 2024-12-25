#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdio.h>

using namespace std;

void printdev(libusb_device *dev){
    libusb_device_descriptor desc; //дескриптор устройства, хранит информацию об устройстве 
    libusb_config_descriptor *config; //дескриптор конфигурации объекта, определяет, как устройство будет работать в различных режимах
    libusb_device_handle *handle = NULL; //дескриптор для #include <iostream>
    unsigned char serial_number[256]; //буфер для хранения серийного номера
    unsigned char manufacturer[256]; // буфер для хранения названия производителя
    unsigned char product[256]; // буфер для хранения названия продукта

    int r = libusb_get_device_descriptor(dev, &desc); //вернет 0 если успешно
    if (r < 0){
        cout << "Ошибка: дескриптор устройства не получен, код: " << r << endl;
        return;
    }

    //получить конфигурацию устройства
    libusb_get_config_descriptor(dev, 0, &config); //устройство для которого хотим получить, индекс конфигурации и куда будет записан адрес дескриптора конфига
    //(int)desc.bNumConfigurations; //- кол-во возможных конфигураций
    //(int)config->bNumInterfaces; //- кол-во доступных интерфейсов
    //Вывод класса:
    int class_usb = (int)desc.bDeviceClass;
    if (class_usb == 0){
        //перебираем все интерфейсы устройства
        for (int i = 0; i < config->bNumInterfaces; ++i) {
            //получаем интрефейс устройства с индексом i
            const libusb_interface& iface = config->interface[i];
            //перебираем все альтернативные настройки для i интерфейса
            for (int ii = 0; ii < iface.num_altsetting; ++ii) {
                //получаем дескриптор альтернативной настройки
                const libusb_interface_descriptor& iface_desc = iface.altsetting[ii];
                //выводим класс интерфейса
                std::cout << "Класс интерфейса: " << static_cast<int>(iface_desc.bInterfaceClass) << std::endl;
            }
        }
    }else{
        cout << "Класс устройства: " << class_usb << endl;
    }

    printf("Идентификатор производителя: %.4x\n", desc.idVendor);
    printf("Идентификатор продукта: %.4x\n", desc.idProduct);

    //открытие устройства для получения серийного номера
    r = libusb_open(dev, &handle);
    if (r < 0) {
        cout << "Ошибка: дескриптор устройства не получен, код: " << r << endl;
    } else {
        // if (desc.iManufacturer) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, manufacturer, sizeof(manufacturer));
            if (r > 0) {
                cout << "Производитель: " << manufacturer << endl;
            } else {
                cout << "Ошибка: не удалось получить название производителя, код: " << r << endl;
            }
        // } else {
            // cout << "Название производителя отсутствует" << endl;
        // }

        if (r) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, product, sizeof(product));
            if (r > 0) {
                cout << "Продукт: " << product << endl;
            } else {
                cout << "Ошибка: не удалось получить название продукта, код: " << r << endl;
            }
        } else {
            cout << "Название продукта отсутствует" << endl;
        }

        // Получение серийного номера, если доступен
        if (desc.iSerialNumber) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial_number, sizeof(serial_number));
            if (r > 0) {
                cout << "Серийный номер: " << serial_number << endl;
            } else {
                cout << "Ошибка: не удалось получить серийный номер, код: " << r << endl;
            }
        } else {
            cout << "Серийный номер отсутствует" << endl;
        }
        libusb_close(handle); // закрытие устройства
    }

    cout << "===========================================================" << endl;
    libusb_free_config_descriptor(config);
}

int main(){
    libusb_device **devs; //указатель на указатель на устройство, используется для получения списка устройств
    libusb_context *ctx = NULL; //контекст сессии libusb
    int r; //для возвращаемых значений
    ssize_t cnt; //число найденных USB-устройств
    ssize_t i; //индексная переменная цикла перебора всех устройств

    //инициализировать библиотеку libusb, открыть сессию работы с libusb
    r = libusb_init(&ctx);
    if (r < 0){
        cout << "Ошибка: дескриптор устройства не получен, код: " << r << endl; //0 - открылось, < 0 - не открылось
        return 1;
    }

    //задать уровень подробности отладочных сообщений
    libusb_set_debug(ctx, 3);
    //более новое но тоже самое
    //libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_NONE);

    //получить список всех найденных USB- устройств
    cnt = libusb_get_device_list(ctx, &devs); //возвращает количество найденных устройств
    if (cnt < 0) {
        cout << "Ошибка: дескриптор устройства не получен, код: " << r << endl;
        return 1;
    }

    cout << "найдено устройств: " << cnt << endl;
    cout << "===========================================================" << endl;

    for (i = 0; i < cnt; i++){ //цикл перебора всех устройств
        printdev(devs[i]); //печать параметров устройства
    }
    //освободить память, выделенную функцией получения списка устройств
    libusb_free_device_list(devs, 1);  //завершить работу с библиотекой libusb,
    libusb_exit(ctx);  //закрыть сессию работы с libusb

    return 0;
}
