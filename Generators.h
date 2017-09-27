#ifndef GENERATORS_H
#define GENERATORS_H

Coordinates genCoordinates()
{
    random_device generator;
    uniform_int_distribution<int> distribution(0, 8);
    int x = 0;
    int y = 0;
    while (x < 2 && y < 2)
    {
        x = distribution(generator);
        y = distribution(generator);
    }
    Coordinates temp(x, y);
    return temp;
}

#endif /* GENERATORS_H */

