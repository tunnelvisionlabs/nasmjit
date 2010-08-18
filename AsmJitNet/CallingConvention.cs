namespace AsmJitNet2
{
    /// <summary>
    /// Calling convention type.
    /// </summary>
    /// <remarks>
    /// Calling convention is scheme how function arguments are passed into 
    /// function and how functions returns values. In assembler programming
    /// it's needed to always comply with function calling conventions, because
    /// even small inconsistency can cause undefined behavior or crash.
    ///
    /// List of calling conventions for 32 bit x86 mode:
    /// - @c CALL_CONV_CDECL - Calling convention for C runtime.
    /// - @c CALL_CONV_STDCALL - Calling convention for WinAPI functions.
    /// - @c CALL_CONV_MSTHISCALL - Calling convention for C++ members under 
    ///      Windows (produced by MSVC and all MSVC compatible compilers).
    /// - @c CALL_CONV_MSFASTCALL - Fastest calling convention that can be used
    ///      by MSVC compiler.
    /// - @c CALL_CONV_BORNANDFASTCALL - Borland fastcall convention.
    /// - @c CALL_CONV_GCCFASTCALL_2 - GCC fastcall convention with 2 register
    ///      arguments.
    /// - @c CALL_CONV_GCCFASTCALL_3 - GCC fastcall convention with 3 register
    ///      arguments.
    ///
    /// List of calling conventions for 64 bit x86 mode (x64):
    /// - @c CALL_CONV_X64W - Windows 64 bit calling convention (WIN64 ABI).
    /// - @c CALL_CONV_X64U - Unix 64 bit calling convention (AMD64 ABI).
    ///
    /// There is also @c CALL_CONV_DEFAULT that is defined to fit best to your 
    /// compiler.
    ///
    /// These types are used together with @c AsmJit::Compiler::newFunction() 
    /// method.
    /// </remarks>
    public enum CallingConvention
    {
        None,
        X64W,
        X64U,
        Cdecl,
        StdCall,
        MsThisCall,
        MsFastCall,
        BorlandFastCall,
        GccFastCall2,
        GccFastCall3,
        Default = Cdecl
    }
}
