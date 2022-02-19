#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <iostream>
#include <functional>
#include <cassert>
#include <stack>
#include "typesystem.h"

class Memory
{
public: 
    enum class Content
        {
         EMPTY,
         NAMED,
         TEMP,
         REFERENCED,
         PROTECTED
        };

private:
    struct Cell
    {
        static std::string const INT_TYPE;
        
        std::string identifier;
        std::string scope;
        Content     content;
        TypeSystem::Type type;
        
        Cell():
            content(Content::EMPTY)
        {}
        
        void clear();
        void backup();
        void restore();
        
        bool empty() const
        {
            return content == Content::EMPTY;
        }
        
        int size() const
        {
            return type.size();
        }

    private:
        using Members = std::tuple<std::string,
                                   std::string,
                                   Content,
                                   TypeSystem::Type
                                   >;

        std::stack<Members> d_backupStack;
    };

    std::vector<Memory::Cell> d_memory;
    std::stack<int> d_protectedStack;
    
public:
    Memory(size_t sz):
        d_memory(sz)
    {}

    size_t size() const;
    int getTemp(std::string const &scope, TypeSystem::Type type);
    int getTemp(std::string const &scope, int const sz = 1);
    int getTempBlock(std::string const &scope, int const sz);
    int allocate(std::string const &ident, std::string const &scope, TypeSystem::Type type);
    
    int find(std::string const &ident, std::string const &scope) const;
    int sizeOf(int const addr) const;
    int sizeOf(std::string const &ident, std::string const &scope) const;
    int pop();
    void push(int const addr);
    void freeTemps(std::string const &scope);
    void freeLocals(std::string const &scope);
    void markAsTemp(int const addr);
    void rename(int const addr, std::string const &ident, std::string const &scope);
    bool isTemp(int const addr) const;
    std::string identifier(int const addr) const;
    std::string scope(int const addr) const;
    TypeSystem::Type type(int const addr) const;
    TypeSystem::Type type(std::string const &ident, std::string const &scope) const;

    
private:    
    int findFree(int sz = 1);
    void place(std::string const &ident, std::string const &scope, TypeSystem::Type type, int const addr);

    template <typename Predicate>
    void freeIf(Predicate &&pred);
};

inline size_t Memory::size() const
{
    return d_memory.size();
}

template <typename Predicate>
void Memory::freeIf(Predicate&& pred)
{
    for (size_t idx = 0; idx != d_memory.size(); ++idx)
    {
        Cell &cell = d_memory[idx];
        if (pred(cell))
        {
            for (int offset = 1; offset < cell.size(); ++offset)
            {
                Cell &referenced = d_memory[idx + offset];
                // assert(referenced.content == Content::REFERENCED &&
                //        "Trying to free a referenced cell that is not marked as referenced");
                
                referenced.clear();
            }
            cell.clear();
        }
    }

}


#endif
