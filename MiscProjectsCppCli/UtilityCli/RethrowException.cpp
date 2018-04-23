//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	RethrowException.cpp

	File Description	:	Unix epoch time support logic.

	Revision History	:	2005-02-28 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2005 - 2018.

		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace UtilityCli {

//	////////////////////////////////////////////////////////////////////////////
/*
	Analogue of MLB::Utility::Rethrow() support.
*/
public ref class ExceptionHelper {
public:
	static void Prep(System::Exception ^except)
	{
		except->GetType()->GetMethod("InternalPreserveStackTrace",
			System::Reflection::BindingFlags::Instance |
			System::Reflection::BindingFlags::NonPublic)->Invoke(except, nullptr);

//		XxxUtility::RethrowPrep(except);
	}

	static void Prep(System::Exception ^except, System::String ^except_string)
	{
		except->GetType()->GetMethod("_message",
			System::Reflection::BindingFlags::Instance |
			System::Reflection::BindingFlags::NonPublic)->
			SetValue(except, except_string);

		Prep(except);

//		XxxUtility::RethrowPrep(except);
	}

	static void Prep(System::Exception ^except, const char *except_string)
	{
		Prep(except, gcnew System::String((except_string) ? except_string : ""));
	}

	static void Prep(System::Exception ^except,
		const std::string &except_string)
	{
		Prep(except, except_string.c_str());
	}
};
//	////////////////////////////////////////////////////////////////////////////

} // namespace UtilityCli
 
} //	namespace MLB

