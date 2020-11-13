#include <vtil/vtil>

/*
 * Demonstrates basic simplification of a small "obfuscated" basic block
 * Deobfuscated this code would just be a series of "push N" instructions where N = {0..100}
 */

int main()
{
    auto block = vtil::basic_block::begin(0x1337);
    vtil::register_desc eax(vtil::register_physical, X86_REG_EAX, 32, 0);

    block->mov(eax, 0);

    for(int i = 0; i < 100; ++i)
    {
        block->add(eax, 13);
        block->nop();
        block->sub(eax, 12);
        block->nop();
        block->add(eax, 14);
        block->mov(eax, eax);
        block->sub(eax, eax);
        block->bxor(eax, i);
        block->push(eax);
    }

    block->vpinr(eax);      // pin register eax as read so it doesn't get optimized away
    block->vexit(0ull); // marks the end of a basic_block

    vtil::logger::log(":: Before:\n");
    vtil::debug::dump(block->owner);

    vtil::logger::log("\n");

    vtil::optimizer::apply_each<
        vtil::optimizer::profile_pass,
        vtil::optimizer::collective_cross_pass
    >{}(block->owner);      // executes all optimization passes

    vtil::logger::log("\n");

    vtil::logger::log(":: After:\n");
    vtil::debug::dump(block->owner);
}