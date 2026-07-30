struct TestBackend
{
    static void write(char c) noexcept
    {
        // collect character
    }
};

using Log = core::BasicLog<TestBackend>;