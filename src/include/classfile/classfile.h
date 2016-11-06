#ifndef H_CLASSFILE
#define H_CLASSFILE 1

#include <stdint.h>

#ifdef INCLUDE_CLASSFILE_SELF_
#define CLASSFILE_EXTERN
#else
#define CLASSFILE_EXTERN extern
#endif

#define CONST_CLASSFILE_MAGIC  0xCAFEBABE

#define CONST_CLASSFILE_ACCESS_PUBLIC  0x0001
#define CONST_CLASSFILE_ACCESS_FINAL  0x0010
#define CONST_CLASSFILE_ACCESS_SUPER  0x0020
#define CONST_CLASSFILE_ACCESS_INTERFACE  0x0200
#define CONST_CLASSFILE_ACCESS_ABSTRACT  0x0400
#define CONST_CLASSFILE_ACCESS_SYNTHETIC  0x1000
#define CONST_CLASSFILE_ACCESS_ANNOTATION  0x2000
#define CONST_CLASSFILE_ACCESS_ENUM  0x4000

#define CONST_CONSTANTPOOLINFO_TAG_CLASS  7
#define CONST_CONSTANTPOOLINFO_TAG_FIELD_REF  9
#define CONST_CONSTANTPOOLINFO_TAG_METHOD_REF  10
#define CONST_CONSTANTPOOLINFO_TAG_INTERFACE_METHOD_REF  11
#define CONST_CONSTANTPOOLINFO_TAG_STRING  8
#define CONST_CONSTANTPOOLINFO_TAG_INTEGER  3
#define CONST_CONSTANTPOOLINFO_TAG_FLOAT  4
#define CONST_CONSTANTPOOLINFO_TAG_LONG  5
#define CONST_CONSTANTPOOLINFO_TAG_DOUBLE  6
#define CONST_CONSTANTPOOLINFO_TAG_NAME_AND_TYPE  12
#define CONST_CONSTANTPOOLINFO_TAG_UTF8  1
#define CONST_CONSTANTPOOLINFO_TAG_METHOD_HANDLE  15
#define CONST_CONSTANTPOOLINFO_TAG_METHOD_TYPE  16
#define CONST_CONSTANTPOOLINFO_TAG_INVOKE_DYNAMIC  18

#define CONST_FIELD_ACCESS_PUBLIC  0x0001
#define CONST_FIELD_ACCESS_PRIVATE  0x0002
#define CONST_FIELD_ACCESS_PROTECTED  0x0004
#define CONST_FIELD_ACCESS_STATIC  0x0008
#define CONST_FIELD_ACCESS_FINAL  0x0010
#define CONST_FIELD_ACCESS_VOLATILE  0x0040
#define CONST_FIELD_ACCESS_TRANSIENT  0x0080
#define CONST_FIELD_ACCESS_SYNTHETIC  0x1000
#define CONST_FIELD_ACCESS_ENUM  0x4000

#define CONST_METHOD_ACCESS_PUBLIC  0x0001
#define CONST_METHOD_ACCESS_PRIVATE  0x0002
#define CONST_METHOD_ACCESS_PROTECTED  0x0004
#define CONST_METHOD_ACCESS_STATIC  0x0008
#define CONST_METHOD_ACCESS_FINAL  0x0010
#define CONST_METHOD_ACCESS_SYNCHRONIZED  0x0020
#define CONST_METHOD_ACCESS_BRIDGE  0x0040
#define CONST_METHOD_ACCESS_VARARGS  0x0080
#define CONST_METHOD_ACCESS_NATIVE  0x0100
#define CONST_METHOD_ACCESS_ABSTRACT  0x0400
#define CONST_METHOD_ACCESS_STRICT  0x0800
#define CONST_METHOD_ACCESS_SYNTHETIC  0x1000

#define CONST_INNERCLASS_ACCESS_PUBLIC  0x0001
#define CONST_INNERCLASS_ACCESS_PRIVATE  0x0002
#define CONST_INNERCLASS_ACCESS_PROTECTED  0x0004
#define CONST_INNERCLASS_ACCESS_STATIC  0x0008
#define CONST_INNERCLASS_ACCESS_FINAL  0x0010
#define CONST_INNERCLASS_ACCESS_INTERFACE  0x0200
#define CONST_INNERCLASS_ACCESS_ABSTRACT  0x0400
#define CONST_INNERCLASS_ACCESS_SYNTHETIC  0x1000
#define CONST_INNERCLASS_ACCESS_ANNOTATION  0x2000
#define CONST_INNERCLASS_ACCESS_ENUM  0x4000

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t constant_value_index;
} Attribute_ConstantValue;

typedef struct{
    uint16_t start_pc;
    uint16_t end_pc;
    uint16_t hanfler_pc;
    uint16_t catch_type;
} ExceptionInfo;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t* code;
    uint16_t exception_table_length;
    ExceptionInfo* exception_table;
    uint16_t attributes_count;
    void* attributes;
} Attribute_Code;

typedef struct{
    uint8_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t exceptions_count;
    uint16_t* exception_indexes;
} Attribute_Exceptions;

typedef struct {
    uint16_t inner_class_info_index;
    uint16_t outer_class_info_index;
    uint16_t inner_name_index;
    uint16_t inner_class_access_flags;
} InnerClassInfo;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t classes_count;
    InnerClassInfo* classes;
} Attribute_InnerClasses;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t class_index;
    uint16_t method_index;
} Attribute_EnclosingMethod;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
} Attribute_Synthetic;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t signature_index;
} Attribute_Signature;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t sourcefile_index;
} Attribute_SourceFile;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t* debug_extension;
} Attribute_SourceDebugExtension;

typedef struct{
    uint16_t start_pc;
    uint16_t line_number;
} LineNumberTableEntry;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t line_number_entries_count;
    LineNumberTableEntry* table;
} Attribute_LineNumberTable;

typedef struct{
    uint16_t start_pc;
    uint16_t length;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t index;
} LocalVariableTableEntry;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t local_variable_entries_count;
    LocalVariableTableEntry* table;
} Attribute_LocalVariableTable;

typedef struct{
    uint16_t start_pc;
    uint16_t length;
    uint16_t name_index;
    uint16_t signature_index;
    uint16_t index;
} LocalVariableTypeTableEntry;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t local_variable_type_entries_count;
    LocalVariableTypeTableEntry* table;
} Attribute_LocalVariableTypeTable;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
} Attribute_Deprecated;

typedef struct _ElementValuePair ElementValuePair;
typedef struct{
    uint16_t type_index;
    uint16_t element_value_pairs_count;
    ElementValuePair* pairs;
} Annotation;

typedef struct _ElementValue ElementValue;
struct _ElementValue{
    uint8_t tag;
    union {
        uint16_t const_value_index;
        struct{
            uint16_t type_name_index;
            uint16_t constant_name_index;
        } enum_const_value;

        uint16_t class_info_index;

        Annotation annotation_value;

        struct{
            uint16_t values_count;
            ElementValue* values;
        } array_value; 
    } value;
};

struct _ElementValuePair{
    uint16_t element_name_index;
    ElementValue value;
};

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t annotations_count;
    Annotation* annotations; 
} Attribute_RuntimeVisibleAnnotations;

typedef struct{
   uint16_t attribute_name_index;
   uint32_t attribute_length;
   uint16_t annotations_count;
   Annotation* annotations;
} Attribute_RuntimeInvisibleAnnotations;

typedef struct{
    uint16_t annotations_count;
    Annotation* annotations;
} ParameterAnnotaion;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t parameters_count;
    ParameterAnnotaion* parameter_annotations;
} Attribute_RuntimeVisibleParameterAnnotations;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t parameters_count;
    ParameterAnnotaion* parameter_annotations;
} Attribute_RuntimeInvisibleParameterAnnotations;

typedef struct{
    uint8_t type_parameter_index;
} TypeParameterTarget;

typedef struct{
    uint16_t supertype_index;
} SuperTypeTarget;

typedef struct{
    uint8_t type_parameter_index;
    uint8_t bound_index;
} TypeParameterBoundTarget;

typedef struct{} EmptyTarget;

typedef struct{
    uint8_t formal_parameter_index;
} FormalParameterTarget;

typedef struct{
    uint16_t throws_type_index;
} ThrowsTarget;

typedef struct{
    uint16_t start_pc;
    uint16_t length;
    uint16_t index;
} LocalvarElement;

typedef struct{
    uint16_t count;
    LocalvarElement* elements;
} LocalvarTarget;

typedef struct{
    uint16_t exception_table_index;
} CatchTarget;

typedef struct{
    uint16_t offset;
} OffsetTarget;

typedef struct{
    uint16_t offset;
    uint8_t type_arugument_index;
} TypeArgumentTarget;

typedef struct{
    uint8_t typepath_kind;
    uint8_t type_arugument_index;
} _Path;

typedef struct{
    uint8_t path_count;
    _Path* path;
} TypePath;

typedef struct{
    uint8_t target_type;
    union {
        TypeParameterTarget type_parameter_target;
        SuperTypeTarget super_type_target;
        TypeParameterBoundTarget type_parameter_bound_target;
        EmptyTarget empty_target;
        FormalParameterTarget formal_parameter_target;
        ThrowsTarget throws_target;
        LocalvarTarget localvar_target;
        CatchTarget catch_target;
        OffsetTarget offset_target;
        TypeArgumentTarget type_arugument_target;
    } target_info;
    TypePath target_path;
    uint16_t type_index;
    uint16_t element_value_pairs_count;
    ElementValuePair* pairs;
} TypeAnnotation;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t annotations_count;
    TypeAnnotation* annotations;
} Attribute_RuntimeVisibleTypeAnnotations;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t annotations_count;
    TypeAnnotation* annotations;
} Attribute_RuntimeInvisibleTypeAnnotations;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    ElementValue default_value;
} Attribute_AnnotationDefault;

typedef struct{
    uint16_t bootstrap_method_ref;
    uint16_t arguments_count;
    uint16_t* arguments;
} BootstrapMethod;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t bootstrap_methods_count;
    BootstrapMethod* bootstrap_methods;
} Attribute_BootstrapMethods;

typedef struct{
    uint16_t name_index;
    uint16_t access_flags;
} _Parameter;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t parameters_count;
    _Parameter* parameters;
} Attribute_MethodParameters;

typedef struct{
// not implemented yet
} StackMapFrame;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t entries_count;
    StackMapFrame* entries;
} Attribute_StackMapTable;

typedef struct{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t* bytes;
} Attribute_Raw;

typedef struct{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    void* attributes;
} FieldInfo;

typedef struct{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    void* attributes;
} MethodInfo;


typedef struct{
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    void** constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    uint16_t* interfaces;
    uint16_t fields_count;
    FieldInfo* fields;
    uint16_t methods_count;
    MethodInfo* methods;
    uint16_t attributes_count;
    void** attributes;
} ClassFile;


typedef struct{
    uint8_t tag;
    uint16_t name_index;
} Constant_ClassInfo;

typedef struct{
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} Constant_FieldRefInfo;

typedef Constant_FieldRefInfo Constant_MethodRefInfo;
typedef Constant_FieldRefInfo Constant_InterfaceMethodRefInfo;

typedef struct{
    uint8_t tag;
    uint16_t string_index;
} Constant_StringInfo;

typedef struct{
    uint8_t tag;
    uint32_t value;
} Constant_IntegerInfo;

typedef struct{
    uint8_t tag;
    float value; // 32 bits
} Constant_FloatInfo;


typedef struct{
    uint8_t tag;
    uint64_t value;
} Constant_LongInfo;

typedef struct{
    uint8_t tag;
    double value; // 64 bits
} Constant_DoubleInfo;

typedef struct{
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
} Constant_NameAndTypeInfo;

typedef struct{
    uint8_t tag;
    uint16_t length;
    uint8_t* bytes;
} Constant_UTF8Info;

typedef struct{
    uint8_t tag;
    uint8_t reference_kind;
    uint16_t reference_index;
} Constant_MethodHandleInfo;

typedef struct{
    uint8_t tag;
    uint16_t descriptor_index;
} Constant_MethodTypeInfo;

typedef struct{
    uint8_t tag;
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
} Constant_InvokeDynamicInfo;

#include "stream.h"
CLASSFILE_EXTERN ClassFile *LoadClassFile(Stream *stream);

#endif
