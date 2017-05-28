
#pragma once

namespace msdfgen {

/// Represents a signed distance and alignment, which together can be compared to uniquely determine the closest edge segment.
class SignedDistance {

public:
#pragma push_macro("INFINITE")
#undef INFINITE
    static const SignedDistance INFINITE;
#pragma pop_macro("INFINITE")

    double distance;
    double dot;

    SignedDistance();
    SignedDistance(double dist, double d);

    friend bool operator<(SignedDistance a, SignedDistance b);
    friend bool operator>(SignedDistance a, SignedDistance b);
    friend bool operator<=(SignedDistance a, SignedDistance b);
    friend bool operator>=(SignedDistance a, SignedDistance b);

};

}
