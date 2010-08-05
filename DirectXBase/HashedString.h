#ifndef HASHEDSTRING
#define HASHEDSTRING

#include <stdlib.h>
#include <Windows.h>
#include <string>

/*
 * Hashed string class by Mike McShaffry. Taken from his book
 * "Game Coding Complete" 3rd Ed.
 */
class HashedString
{
public:
    explicit HashedString(char const * const pIdentStr)
        : ident(hash_name(pIdentStr) ),
          identStr(pIdentStr)
    {}
    
    unsigned long getHashValue() const
    {
        return reinterpret_cast<unsigned long>(ident);
    }

    const std::string getStr() const { return identStr; }

    bool operator< (HashedString const & o) const
    {
        bool r = (getHashValue() < o.getHashValue());
        return r;
    }

    bool operator== (HashedString const & o) const
    {
        bool r = (getHashValue() == o.getHashValue());
        return r;
    }

    static void * hash_name(char const * pIdentStr);

private:
    void* ident;
    std::string identStr;
};

#endif