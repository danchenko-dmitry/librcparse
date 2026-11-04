#include <cstdint>
#include <cstddef>

#define crclen 256

namespace crsf {


uint8_t crc8tab[crclen];
static bool crcTableInitialized = false;

void initCrc(uint8_t poly)
{
    if (crcTableInitialized) return;
    uint8_t crc;

    for (uint16_t i = 0; i < crclen; i++)
    {
        crc = i;
        for (uint8_t j = 0; j < 8; j++)
        {
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        crc8tab[i] = crc & 0xFF;
    }
}

// uint8_t ICACHE_RAM_ATTR calc(const uint8_t data)
// {
//     return crc8tab[data];
// }

uint8_t calcCrc(const uint8_t *data, size_t len, uint8_t crc=0)
{
    while (len--)
    {
        crc = crc8tab[crc ^ *data++];
    }
    return crc;
}

}