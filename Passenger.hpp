class Passenger
{
private:
    /* data */
public:
    Passenger(int, int);
    ~Passenger();
    bool isIn;
    int floorIn;
    int floorOut;
};

Passenger::Passenger(int in, int out)
{
    isIn = false;
    floorIn = in;
    floorOut = out;
}

Passenger::~Passenger()
{
}
