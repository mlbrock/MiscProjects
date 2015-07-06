// ////////////////////////////////////////////////////////////////////////////
// From message.h
class LIBPROTOBUF_EXPORT Reflection {
   // ...
 protected:
  // Obtain a pointer to a Repeated Field Structure and do some type checking:
  //   on field->cpp_type(),
  //   on field->field_option().ctype() (if ctype >= 0)
  //   of field->message_type() (if message_type != NULL).
  // We use 1 routine rather than 4 (const vs mutable) x (scalar vs pointer).
  virtual void* MutableRawRepeatedField(
      Message* message, const FieldDescriptor* field, FieldDescriptor::CppType,
      int ctype, const Descriptor* message_type) const = 0;

 private:
  // Special version for specialized implementations of string.  We can't call
  // MutableRawRepeatedField directly here because we don't have access to
  // FieldOptions::* which are defined in descriptor.pb.h.  Including that
  // file here is not possible because it would cause a circular include cycle.
  void* MutableRawRepeatedString(
      Message* message, const FieldDescriptor* field, bool is_string) const;

   // ...
};
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
// From message.h
#define DECLARE_GET_REPEATED_FIELD(TYPE)                         \
template<>                                                       \
LIBPROTOBUF_EXPORT                                               \
const RepeatedField<TYPE>& Reflection::GetRepeatedField<TYPE>(   \
    const Message& message, const FieldDescriptor* field) const; \
                                                                 \
template<>                                                       \
RepeatedField<TYPE>* Reflection::MutableRepeatedField<TYPE>(     \
    Message* message, const FieldDescriptor* field) const;

DECLARE_GET_REPEATED_FIELD(int32)
DECLARE_GET_REPEATED_FIELD(int64)
DECLARE_GET_REPEATED_FIELD(uint32)
DECLARE_GET_REPEATED_FIELD(uint64)
DECLARE_GET_REPEATED_FIELD(float)
DECLARE_GET_REPEATED_FIELD(double)
DECLARE_GET_REPEATED_FIELD(bool)

#undef DECLARE_GET_REPEATED_FIELD

// =============================================================================
// Implementation details for {Get,Mutable}RawRepeatedPtrField.  We provide
// specializations for <string>, <StringPieceField> and <Message> and handle
// everything else with the default template which will match any type having
// a method with signature "static const google::protobuf::Descriptor* descriptor()".
// Such a type presumably is a descendant of google::protobuf::Message.

template<>
inline const RepeatedPtrField<string>& Reflection::GetRepeatedPtrField<string>(
    const Message& message, const FieldDescriptor* field) const {
  return *static_cast<RepeatedPtrField<string>* >(
      MutableRawRepeatedString(const_cast<Message*>(&message), field, true));
}

template<>
inline RepeatedPtrField<string>* Reflection::MutableRepeatedPtrField<string>(
    Message* message, const FieldDescriptor* field) const {
  return static_cast<RepeatedPtrField<string>* >(
      MutableRawRepeatedString(message, field, true));
}


// -----

template<>
inline const RepeatedPtrField<Message>& Reflection::GetRepeatedPtrField(
    const Message& message, const FieldDescriptor* field) const {
  return *static_cast<RepeatedPtrField<Message>* >(
      MutableRawRepeatedField(const_cast<Message*>(&message), field,
          FieldDescriptor::CPPTYPE_MESSAGE, -1,
          NULL));
}

template<>
inline RepeatedPtrField<Message>* Reflection::MutableRepeatedPtrField(
    Message* message, const FieldDescriptor* field) const {
  return static_cast<RepeatedPtrField<Message>* >(
      MutableRawRepeatedField(message, field,
          FieldDescriptor::CPPTYPE_MESSAGE, -1,
          NULL));
}

template<typename PB>
inline const RepeatedPtrField<PB>& Reflection::GetRepeatedPtrField(
    const Message& message, const FieldDescriptor* field) const {
  return *static_cast<RepeatedPtrField<PB>* >(
      MutableRawRepeatedField(const_cast<Message*>(&message), field,
          FieldDescriptor::CPPTYPE_MESSAGE, -1,
          PB::default_instance().GetDescriptor()));
}

template<typename PB>
inline RepeatedPtrField<PB>* Reflection::MutableRepeatedPtrField(
    Message* message, const FieldDescriptor* field) const {
  return static_cast<RepeatedPtrField<PB>* >(
      MutableRawRepeatedField(message, field,
          FieldDescriptor::CPPTYPE_MESSAGE, -1,
          PB::default_instance().GetDescriptor()));
// ////////////////////////////////////////////////////////////////////////////

