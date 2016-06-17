//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	RvHandleSupport.cpp

	File Description	:	Tib/Rv handle support use from .NET.

	Revision History	:	2005-02-28 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 20o5 - 2016.

		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace RvUtilXCli {

//	////////////////////////////////////////////////////////////////////////////
public ref class RvHandleSupport {
private:
	template <typename RvType> System::IntPtr ExtractRvCTypeIntPtrHelper(
		RvType ^src, const char *field_name)
	{
		return(((System::IntPtr) src->GetType()->GetField(field_name,
			((System::Reflection::BindingFlags)
			 (System::Reflection::BindingFlags::Instance |
			  System::Reflection::BindingFlags::NonPublic)))->GetValue(src)));
	}

public:
/*
	static System::IntPtr ExtractRvCTypeIntPtr(TIBCO::Rendezvous::Message ^src)
	{
		return(((System::IntPtr) src->GetType()->GetField("nativeMessage",
			((System::Reflection::BindingFlags)
			 (System::Reflection::BindingFlags::Instance |
			  System::Reflection::BindingFlags::NonPublic)))->GetValue(src)));

		return(ExtractRvCTypeIntPtrHelper(src, field_name));
	}

	static tibrvMsg ExtractCType(TIBCO::Rendezvous::Message ^src)
	{
		return(static_cast<tibrvMsg>(ExtractRvCTypeIntPtr(src)));
	}
*/
	static tibrvMsg ExtractCType(TIBCO::Rendezvous::Message ^src)
	{
		return(static_cast<tibrvMsg>(
			ExtractRvCTypeIntPtrHelper(src, "nativeMessage")));
	}

	static tibrvTransport ExtractCType(TIBCO::Rendezvous::NetTransport ^src)
	{
		return(static_cast<tibrvTransport>(
			ExtractRvCTypeIntPtrHelper(src, "id")));
	}

	static tibrvQueue ExtractCType(TIBCO::Rendezvous::NetQueue ^src)
	{
		return(static_cast<tibrvQueue>(ExtractRvCTypeIntPtrHelper(src, "id")));
	}
};
//	////////////////////////////////////////////////////////////////////////////

} // namespace RvUtilXCli
 
} //	namespace MLB

