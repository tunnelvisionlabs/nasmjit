namespace AsmJitNet
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

        /// <summary>
        /// Windows 64-bit calling convention
        /// </summary>
        X64W,

        /// <summary>
        /// Unix 64-bit calling convention
        /// </summary>
        X64U,

        /// <summary>
        /// Calling convention for C runtime
        /// </summary>
        Cdecl,

        /// <summary>
        /// Calling convention for WinAPI functions
        /// </summary>
        StdCall,

        /// <summary>
        /// Calling convention for C++ members under Windows (produced by MSVC and all MSVC compatible compilers)
        /// </summary>
        MsThisCall,

        /// <summary>
        /// Fastest calling convention that can be used by MSVC compiler
        /// </summary>
        MsFastCall,

        /// <summary>
        /// Borland fastcall convention
        /// </summary>
        BorlandFastCall,

        /// <summary>
        /// GCC-specific fastcall convention.
        /// </summary>
        /// <remarks>
        /// Two first parameters (evaluated from left-to-right) are in ECX:EDX 
        /// registers, all others on the stack in right-to-left order.
        ///
        /// Arguments direction:
        /// - Right to Left (except to first two integer arguments in ECX:EDX)
        ///
        /// Stack is cleaned by:
        /// - Callee.
        ///
        /// Return value:
        /// - Integer types - EAX:EDX registers.
        /// - Floating points - st(0) register.
        ///
        /// @note This calling convention should be compatible to
        /// @c CALL_CONV_MSFASTCALL.
        /// </remarks>
        GccFastCall,

        /// <summary>
        /// GCC specific regparm(1) convention.
        /// </summary>
        /// <remarks>
        /// The first parameter (evaluated from left-to-right) is in EAX register,
        /// all others on the stack in right-to-left order.
        ///
        /// Arguments direction:
        /// - Right to Left (except to first one integer argument in EAX)
        ///
        /// Stack is cleaned by:
        /// - Caller.
        ///
        /// Return value:
        /// - Integer types - EAX:EDX registers.
        /// - Floating points - st(0) register.
        /// </remarks>
        GccRegParm1,

        /// <summary>
        /// GCC specific regparm(2) convention.
        /// </summary>
        /// <remarks>
        /// Two first parameters (evaluated from left-to-right) are in EAX:EDX 
        /// registers, all others on the stack in right-to-left order.
        ///
        /// Arguments direction:
        /// - Right to Left (except to first two integer arguments in EAX:EDX)
        ///
        /// Stack is cleaned by:
        /// - Caller.
        ///
        /// Return value:
        /// - Integer types - EAX:EDX registers.
        /// - Floating points - st(0) register.
        /// </remarks>
        GccRegParm2,

        /// <summary>
        /// GCC specific fastcall with 3 parameters in registers.
        /// </summary>
        /// <remarks>
        /// Three first parameters (evaluated from left-to-right) are in 
        /// EAX:EDX:ECX registers, all others on the stack in right-to-left order.
        ///
        /// Arguments direction:
        /// - Right to Left (except to first three integer arguments in EAX:EDX:ECX)
        ///
        /// Stack is cleaned by:
        /// - Caller.
        ///
        /// Return value:
        /// - Integer types - EAX:EDX registers.
        /// - Floating points - st(0) register.
        /// </remarks>
        GccRegParm3,

        /// <summary>
        /// Specifier for the default calling convention for the current platform
        /// </summary>
        Default,
    }
}
