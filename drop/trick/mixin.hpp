class Name
{
public:
    Name(std::string firstName, std::string lastName)
        : firstName_(std::move(firstName))
        , lastName_(std::move(lastName)) {}

    void print() const
    {
        std::cout << lastName_ << ", " << firstName_ << '\n';
    }

private:
    std::string firstName_;
    std::string lastName_;
};

template<typename Printable>
struct RepeatPrint : Printable
{
    explicit RepeatPrint(Printable const& printable) : Printable(printable) {}
    void repeat(unsigned int n) const
    {
        while (n-- > 0)
        {
            this->print();
        }
    }
};

template<typename Printable>
RepeatPrint<Printable> make_repeat_print(Printable const& printable)
{
    return RepeatPrint<Printable>(printable);
}

//Name ned("Eddard", "Stark");
//repeatPrint(ned).repeat(10);