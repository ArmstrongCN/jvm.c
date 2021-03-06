#include<stdarg.h>

#include "slog.h"
#include "gc.h"


#define INCLUDE_CLASSFILE_SELF 1
#include "classfile/classfile.h"
#include "classfile/op.h"
#include "stream.h"

#include "utils.h"


static void* parseConstantPool(Stream *stream, uint16_t size);
static Constant_ClassInfo* cp_parse_classInfo(Stream *stream);
static Constant_FieldRefInfo* cp_parse_filedRefInfo(Stream* stream);
static Constant_MethodRefInfo* cp_parse_methodRefInfo(Stream* stream);
static Constant_InterfaceMethodRefInfo* cp_parse_interfaceMethodRefInfo(Stream* stream);
static Constant_StringInfo* cp_parse_stringInfo(Stream* stream);
static Constant_IntegerInfo* cp_parse_intergerInfo(Stream* stream);
static Constant_FloatInfo* cp_parse_floatInfo(Stream* stream);
static Constant_LongInfo* cp_parse_longInfo(Stream* stream);
static Constant_DoubleInfo* cp_parse_doubleInfo(Stream* stream);
static Constant_NameAndTypeInfo* cp_parse_nameAndTypeInfo(Stream* stream);
static Constant_UTF8Info* cp_parse_utf8Info(Stream* stream);
static Constant_MethodHandleInfo* cp_parse_methodHandleInfo(Stream* stream);
static Constant_MethodTypeInfo* cp_parse_methodTypeInfo(Stream* stream);
static Constant_InvokeDynamicInfo* cp_parse_invokeDynamicInfo(Stream* stream);
static void* parseFields(Stream *stream, ClassFile *classfile, uint16_t count);
static void* parseMethods(Stream *stream, ClassFile *classfile, uint16_t count);
static void* parseAttributes(Stream *stream, ClassFile *classfile, uint16_t count);
static Attribute_ConstantValue* attr_parse_constantValue(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_Code* attr_parse_code(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_StackMapTable* attr_parse_stackMapTable(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_Exceptions* attr_parse_exceptions(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_InnerClasses* attr_parse_innerClasses(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_EnclosingMethod* attr_parse_enclosingMethod(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_Synthetic* attr_parse_synthetic(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_Signature* attr_parse_signature(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_SourceFile* attr_parse_sourceFile(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_SourceDebugExtension* attr_parse_sourceDebugExtention(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_LineNumberTable* attr_parse_lineNumberTable(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_LocalVariableTable* attr_parse_localVariableTable(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_LocalVariableTypeTable* attr_parse_localVariableTypeTable(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_Deprecated* attr_parse_deprecated(Stream *stream, ClassFile *classfile, uint16_t name_index);
static ElementValue* parseElementValue(Stream *stream, ClassFile *classfile, ElementValue *value);
static Annotation* parseAnnotation(Stream *stream, ClassFile *classfile, Annotation *annotation);
static Attribute_RuntimeVisibleAnnotations* attr_parse_runtimeVisibleAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_RuntimeInvisibleAnnotations* attr_parse_runtimeInvisibleAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_RuntimeVisibleParameterAnnotations* attr_parse_runtimeVisibleParameterAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_RuntimeInvisibleParameterAnnotations* attr_parse_runtimeInvisibleParameterAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index);
static TypeAnnotation* parseTypeAnnotation(Stream *stream, ClassFile *classfile, TypeAnnotation *annotation);
static Attribute_RuntimeVisibleTypeAnnotations* attr_parse_runtimeVisibleTypeAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_RuntimeInvisibleTypeAnnotations* attr_parse_runtimeInvisibleTypeAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_AnnotationDefault* attr_parse_annotationDefault(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_BootstrapMethods* attr_parse_bootstrapMethods(Stream *stream, ClassFile *classfile, uint16_t name_index);
static Attribute_MethodParameters* attr_parse_methodParameters(Stream *stream, ClassFile *classfile, uint16_t name_index);



ClassFile *LoadClassFile(Stream *stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	if(NULL==reader){
		error("Reading ClassFile Error. Not a readable stream.");
		return NULL;
	}
	ClassFile * classfile = (ClassFile*)GC_malloc(sizeof(ClassFile));
	if(0<=reader->ReadUint32(stream, &classfile->magic)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	if(classfile->magic!=CONST_CLASSFILE_MAGIC){
		error("Reading ClassFile Error. Invalid Magic.");
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &classfile->minor_version)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &classfile->major_version)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	switch(classfile->major_version){
		case 45: break;
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
				 if(0==classfile->minor_version){
					 break;
				 }
		default:
				 error("Reading ClassFile Error. Unsupported Class Version");
				 return NULL;
	}

	if(0<=reader->ReadUint16(stream, &classfile->constant_pool_count)){
		error("Reading ClassFile Error.");
		return NULL;
	}

    // load constant pool
	void *cp = parseConstantPool(stream,classfile->constant_pool_count);
	if(NULL==cp){
		error("Reading ClassFile Error.");
		return NULL;
	}
	classfile->constant_pool = cp;

	if(0<=reader->ReadUint16(stream, &classfile->access_flags)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &classfile->this_class)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &classfile->super_class)){
		error("Reading ClassFile Error.");
		return NULL;
	}

	if(0<=reader->ReadUint16(stream, &classfile->interfaces_count)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	uint16_t* interfaces = (uint16_t*)GC_malloc(sizeof(uint16_t)*classfile->interfaces_count);
	for(int i=0;i<classfile->interfaces_count;i++){
		if(0<=reader->ReadUint16(stream, interfaces+i)){
			error("Reading ClassFile Error.");
			return NULL;
		}
	}

	if(0<=reader->ReadUint16(stream, &classfile->fields_count)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	void *fields = parseFields(stream,classfile,classfile->fields_count);
	if(NULL==fields){
		error("Reading ClassFile Error.");
		return NULL;
	}
	classfile->fields = fields;

	if(0<=reader->ReadUint16(stream, &classfile->methods_count)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	void *methods = parseMethods(stream, classfile, classfile->methods_count);
	if(NULL==methods){
		error("Reading ClassFile Error.");
		return NULL;
	}
	classfile->methods = methods;

	if(0<=reader->ReadUint16(stream, &classfile->attributes_count)){
		error("Reading ClassFile Error.");
		return NULL;
	}
	classfile->attributes = parseAttributes(stream,classfile, classfile->attributes_count);
	if(NULL==classfile->attributes){
		error("Reading ClassFile Error.");
		return NULL;
	}
	return classfile;
}

static void* parseConstantPool(Stream *stream, uint16_t size){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	void **array = GC_malloc(sizeof(void*)*size);
    // element 0 is invalid.
	array[0]=NULL;
	for(int i=1;i<size;i++){
		uint8_t tag=0;
		if(0<=reader->ReadUint8(stream,&tag)){
			return NULL;
		}
		switch(tag){
			case CONST_CONSTANTPOOLINFO_TAG_CLASS: 
				array[i] = (void*)cp_parse_classInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_FIELD_REF: 
				array[i] = (void*)cp_parse_filedRefInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_METHOD_REF: 
				array[i] = (void*)cp_parse_methodRefInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_INTERFACE_METHOD_REF: 
				array[i] = (void*)cp_parse_interfaceMethodRefInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_STRING: 
				array[i] = (void*)cp_parse_stringInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_INTEGER: 
				array[i] = (void*)cp_parse_intergerInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_FLOAT: 
				array[i] = (void*)cp_parse_floatInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_LONG: 
				array[i] = (void*)cp_parse_longInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				i++;
				break;
			case CONST_CONSTANTPOOLINFO_TAG_DOUBLE: 
				array[i] = (void*)cp_parse_doubleInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				i++;
				break;
			case CONST_CONSTANTPOOLINFO_TAG_NAME_AND_TYPE: 
				array[i] = (void*)cp_parse_nameAndTypeInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_UTF8: 
				array[i] = (void*)cp_parse_utf8Info(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_METHOD_HANDLE: 
				array[i] = (void*)cp_parse_methodHandleInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_METHOD_TYPE: 
				array[i] = (void*)cp_parse_methodTypeInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;
			case CONST_CONSTANTPOOLINFO_TAG_INVOKE_DYNAMIC: 
				array[i] = (void*)cp_parse_invokeDynamicInfo(stream);
				if(NULL==array[i]){
					error("Reading ClassFile Error.");
					return NULL;
				}
				break;

			default:
				error("Reading ClassFile Error. Unknown constant info tag: %d at position: %ld.", tag, reader->Position(stream));
				return NULL;
		}
	}
	return array;
}

static Constant_ClassInfo* cp_parse_classInfo(Stream *stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_ClassInfo *classinfo = (Constant_ClassInfo*)GC_malloc(sizeof(Constant_ClassInfo));
	classinfo->tag = CONST_CONSTANTPOOLINFO_TAG_CLASS;
	if(0<=reader->ReadUint16(stream, &classinfo->name_index)){
		error("Reading ConstantPool Error. EOF of name_index at position %ld.", reader->Position(stream));
		return NULL;
	}
	return classinfo;
}

static Constant_FieldRefInfo* cp_parse_filedRefInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_FieldRefInfo * fieldRef = (Constant_FieldRefInfo*)GC_malloc(sizeof(Constant_FieldRefInfo));
	fieldRef->tag = CONST_CONSTANTPOOLINFO_TAG_FIELD_REF;
	if(0<=reader->ReadUint16(stream, &fieldRef->class_index)){
		error("Reading ConstantPool Error. EOF of class_index at position %ld.", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &fieldRef->name_and_type_index)){
		error("Reading ConstantPool Error. EOF of name_and_type_index %ld.", reader->Position(stream));
		return NULL;
	}
	return fieldRef;
}

static Constant_MethodRefInfo* cp_parse_methodRefInfo(Stream* stream){
	Constant_MethodRefInfo* methodRef = (Constant_MethodRefInfo*)cp_parse_filedRefInfo(stream);
	methodRef->tag = CONST_CONSTANTPOOLINFO_TAG_METHOD_REF;
	return methodRef;
}

static Constant_InterfaceMethodRefInfo* cp_parse_interfaceMethodRefInfo(Stream* stream){
	Constant_InterfaceMethodRefInfo* methodRef = (Constant_InterfaceMethodRefInfo*)cp_parse_filedRefInfo(stream);
	methodRef->tag = CONST_CONSTANTPOOLINFO_TAG_INTERFACE_METHOD_REF;
	return methodRef;
}

static Constant_StringInfo* cp_parse_stringInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_StringInfo * stringinfo = (Constant_StringInfo*)GC_malloc(sizeof(Constant_StringInfo));
	stringinfo->tag = CONST_CONSTANTPOOLINFO_TAG_STRING;
	if(0<=reader->ReadUint16(stream, &stringinfo->string_index)){
		error("Reading ConstantPool Error. EOF of string_index at position %ld.", reader->Position(stream));
		return NULL;
	}
	return stringinfo;
}

static Constant_IntegerInfo* cp_parse_intergerInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_IntegerInfo * integerInfo = (Constant_IntegerInfo*)GC_malloc(sizeof(Constant_IntegerInfo));
	integerInfo->tag = CONST_CONSTANTPOOLINFO_TAG_INTEGER;
	if(0<=reader->ReadUint32(stream, &integerInfo->value)){
		error("Reading ConstantPool Error. EOF of bytes at position %ld.", reader->Position(stream));
		return NULL;
	}
	return integerInfo;
}

static Constant_FloatInfo* cp_parse_floatInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_FloatInfo * floatInfo = (Constant_FloatInfo*)GC_malloc(sizeof(Constant_FloatInfo));
	floatInfo->tag = CONST_CONSTANTPOOLINFO_TAG_FLOAT;
	uint32_t value;
	if(0<=reader->ReadUint32(stream, &value)){
		error("Reading ConstantPool Error. EOF of bytes at position %ld.", reader->Position(stream));
		return NULL;
	}
	floatInfo->value = ieee754_bin2float(value);
	return floatInfo;
}

static Constant_LongInfo* cp_parse_longInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_LongInfo * longInfo = (Constant_LongInfo*)GC_malloc(sizeof(Constant_LongInfo));
	longInfo->tag = CONST_CONSTANTPOOLINFO_TAG_LONG;
	if(0<=reader->ReadUint64(stream, &longInfo->value)){
		error("Reading ConstantPool Error. EOF of bytes at position %ld.", reader->Position(stream));
		return NULL;
	}
	return longInfo;
}

static Constant_DoubleInfo* cp_parse_doubleInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_DoubleInfo * doubleInfo = (Constant_DoubleInfo*)GC_malloc(sizeof(Constant_DoubleInfo));
	doubleInfo->tag = CONST_CONSTANTPOOLINFO_TAG_DOUBLE;
	uint64_t value;
	if(0<=reader->ReadUint64(stream, &value)){
		error("Reading ConstantPool Error. EOF of bytes at position %ld.", reader->Position(stream));
		return NULL;
	}
	doubleInfo->value = ieee754_bin2double(value);
	return doubleInfo;
}

static Constant_NameAndTypeInfo* cp_parse_nameAndTypeInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_NameAndTypeInfo * name_type = (Constant_NameAndTypeInfo*)GC_malloc(sizeof(Constant_NameAndTypeInfo));
	name_type->tag = CONST_CONSTANTPOOLINFO_TAG_NAME_AND_TYPE;
	if(0<=reader->ReadUint16(stream, &name_type->name_index)){
		error("Reading ConstantPool Error. EOF of name_index at position %ld.", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &name_type->descriptor_index)){
		error("Reading ConstantPool Error. EOF of descriptor_index %ld.", reader->Position(stream));
		return NULL;
	}
	return name_type;
}

static Constant_UTF8Info* cp_parse_utf8Info(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_UTF8Info * utf8 = (Constant_UTF8Info*)GC_malloc(sizeof(Constant_UTF8Info));
	utf8->tag = CONST_CONSTANTPOOLINFO_TAG_UTF8;
	if(0<=reader->ReadUint16(stream, &utf8->length)){
		error("Reading ConstantPool Error. EOF of length at position %ld.", reader->Position(stream));
		return NULL;
	}
    // modified UTF-8
	utf8->bytes = (uint8_t*)GC_malloc(sizeof(uint8_t)*(utf8->length+1));
	if(0<=reader->ReadBytes(stream, utf8->length, utf8->bytes)){
		error("Reading ConstantPool Error. EOF of bytes %ld.", reader->Position(stream));
		return NULL;
	}
	utf8->bytes[utf8->length] = 0;
	return utf8;
}

static Constant_MethodHandleInfo* cp_parse_methodHandleInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_MethodHandleInfo * methodHandleInfo = (Constant_MethodHandleInfo*)GC_malloc(sizeof(Constant_MethodHandleInfo));
	methodHandleInfo->tag = CONST_CONSTANTPOOLINFO_TAG_METHOD_HANDLE;
	if(0<=reader->ReadUint8(stream, &methodHandleInfo->reference_kind)){
		error("Reading ConstantPool Error. EOF of reference_kind at position %ld.", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &methodHandleInfo->reference_index)){
		error("Reading ConstantPool Error. EOF of reference_index %ld.", reader->Position(stream));
		return NULL;
	}
	return methodHandleInfo;
}

static Constant_MethodTypeInfo* cp_parse_methodTypeInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_MethodTypeInfo* methodTypeInfo = (Constant_MethodTypeInfo*)GC_malloc(sizeof(Constant_MethodTypeInfo));
	methodTypeInfo->tag = CONST_CONSTANTPOOLINFO_TAG_METHOD_TYPE;
	if(0<=reader->ReadUint16(stream, &methodTypeInfo->descriptor_index)){
		error("Reading ConstantPool Error. EOF of descriptor_index at position %ld.", reader->Position(stream));
		return NULL;
	}
	return methodTypeInfo;
}

static Constant_InvokeDynamicInfo* cp_parse_invokeDynamicInfo(Stream* stream){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Constant_InvokeDynamicInfo * invokeDynamicInfo = (Constant_InvokeDynamicInfo*)GC_malloc(sizeof(Constant_InvokeDynamicInfo));
	invokeDynamicInfo->tag = CONST_CONSTANTPOOLINFO_TAG_INVOKE_DYNAMIC;
	if(0<=reader->ReadUint16(stream, &invokeDynamicInfo->bootstrap_method_attr_index)){
		error("Reading ConstantPool Error. EOF of bootstrap_method_attr_index at position %ld.", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &invokeDynamicInfo->name_and_type_index)){
		error("Reading ConstantPool Error. EOF of name_and_type_index %ld.", reader->Position(stream));
		return NULL;
	}
	return invokeDynamicInfo;
}

static void* parseFields(Stream *stream, ClassFile *classfile, uint16_t count){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	FieldInfo *array = GC_malloc(sizeof(FieldInfo)*count);
	for(int i=0;i<count;i++){
		FieldInfo *fieldInfo = &array[i];
		if(0<=reader->ReadUint16(stream, &fieldInfo->access_flags)){
			error("Reading ConstantPool Error. EOF of access_flags %ld.", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &fieldInfo->name_index)){
			error("Reading ConstantPool Error. EOF of name_index %ld.", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &fieldInfo->descriptor_index)){
			error("Reading ConstantPool Error. EOF of descriptor_index %ld.", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &fieldInfo->attributes_count)){
			error("Reading ConstantPool Error. EOF of attributes_count %ld.", reader->Position(stream));
			return NULL;
		}
		fieldInfo->attributes = parseAttributes(stream, classfile, fieldInfo->attributes_count);
		if(NULL==fieldInfo->attributes){
			return NULL;
		}
	}
	return array;
}

static void* parseMethods(Stream *stream, ClassFile *classfile, uint16_t count){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	MethodInfo *array = GC_malloc(sizeof(MethodInfo)*count);
	for(int i=0;i<count;i++){
		MethodInfo *methodInfo = &array[i];
		if(0<=reader->ReadUint16(stream, &methodInfo->access_flags)){
			error("Reading ConstantPool Error. EOF of access_flags %ld.", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &methodInfo->name_index)){
			error("Reading ConstantPool Error. EOF of name_index %ld.", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &methodInfo->descriptor_index)){
			error("Reading ConstantPool Error. EOF of descriptor_index %ld.", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &methodInfo->attributes_count)){
			error("Reading ConstantPool Error. EOF of attributes_count %ld.", reader->Position(stream));
			return NULL;
		}
		methodInfo->attributes = parseAttributes(stream, classfile, methodInfo->attributes_count);
		if(NULL==methodInfo->attributes){
			return NULL;
		}
	}
	return array;
}

static void* parseAttributes(Stream *stream, ClassFile *classfile, uint16_t count){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	void **array = GC_malloc(sizeof(void*)*count);
	for(int i=0;i<count;i++){
		uint16_t name_index;
		if(0<=reader->ReadUint16(stream, &name_index)){
			error("Reading ConstantPool Error. EOF of name_index %ld.", reader->Position(stream));
			return NULL;
		}
		if(name_index==0 || name_index>=classfile->constant_pool_count){
			error("Reading Attributes Error. Invalid name_index %d, items count %d.", name_index,classfile->constant_pool_count);
			return NULL;
		}
		uint8_t* utf8 = CLZFILE_cp_getUTF8(classfile->constant_pool, name_index);
		if(utf8ascii_equals(utf8, "ConstantValue")){
			array[i]=(void*)attr_parse_constantValue(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "Code")){
			array[i]=(void*)attr_parse_code(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "StackMapTable")){
			array[i]=(void*)attr_parse_stackMapTable(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "Exceptions")){
			array[i]=(void*)attr_parse_exceptions(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "InnerClasses")){
			array[i]=(void*)attr_parse_innerClasses(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "EnclosingMethod")){
			array[i]=(void*)attr_parse_enclosingMethod(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "Synthetic")){
			array[i]=(void*)attr_parse_synthetic(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "Signature")){
			array[i]=(void*)attr_parse_signature(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "SourceFile")){
			array[i]=(void*)attr_parse_sourceFile(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "SourceDebugExtention")){
			array[i]=(void*)attr_parse_sourceDebugExtention(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "LineNumberTable")){
			array[i]=(void*)attr_parse_lineNumberTable(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "LocalVariableTable")){
			array[i]=(void*)attr_parse_localVariableTable(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "LocalVariableTypeTable")){
			array[i]=(void*)attr_parse_localVariableTypeTable(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "Deprecated")){
			array[i]=(void*)attr_parse_deprecated(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "RuntimeVisibleAnnotations")){
			array[i]=(void*)attr_parse_runtimeVisibleAnnotations(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "RuntimeInvisibleAnnotations")){
			array[i]=(void*)attr_parse_runtimeInvisibleAnnotations(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "RuntimeVisibleParameterAnnotations")){
			array[i]=(void*)attr_parse_runtimeVisibleParameterAnnotations(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "RuntimeInvisibleParameterAnnotations")){
			array[i]=(void*)attr_parse_runtimeInvisibleParameterAnnotations(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "RuntimeVisibleTypeAnnotations")){
			array[i]=(void*)attr_parse_runtimeVisibleTypeAnnotations(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "RuntimeInvisibleTypeAnnotations")){
			array[i]=(void*)attr_parse_runtimeInvisibleTypeAnnotations(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "AnnotationDefault")){
			array[i]=(void*)attr_parse_annotationDefault(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "AnnotationDefault")){
			array[i]=(void*)attr_parse_annotationDefault(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "BootstrapMethods")){
			array[i]=(void*)attr_parse_bootstrapMethods(stream,classfile,name_index);
		}
		else if(utf8ascii_equals(utf8, "MethodParameters")){
			array[i]=(void*)attr_parse_methodParameters(stream,classfile,name_index);
		}else{
            error("Unknown attribute: %s.",utf8);
            return NULL;
        }
		if(NULL==array[i]){
			return NULL;
		}
	}    
}

static Attribute_ConstantValue* attr_parse_constantValue(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_ConstantValue* constant = (Attribute_ConstantValue*) GC_malloc(sizeof(Attribute_ConstantValue));
	constant->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &constant->attribute_length)){
		error("ConstantValue Attribute", reader->Position(stream));
		return NULL;
	}
	if(constant->attribute_length!=2){
		error("ConstantValue Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &constant->constant_value_index)){
		error("ConstantValue Attribute", reader->Position(stream));
		return NULL;
	}
	if(constant->constant_value_index==0 || constant->constant_value_index>=classfile->constant_pool_count){
		error("ConstantValue Attribute", reader->Position(stream));
		return NULL;
	}
}

static Attribute_Code* attr_parse_code(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_Code* code = (Attribute_Code*) GC_malloc(sizeof(Attribute_Code));
	code->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &code->attribute_length)){
		error("Code Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &code->max_stack)){
		error("Code Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &code->max_locals)){
		error("Code Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint32(stream, &code->code_length)){
		error("Code Attribute", reader->Position(stream));
		return NULL;
	}

	uint8_t *code_bytes = (uint8_t*)GC_malloc(sizeof(uint8_t)*code->code_length);
	if(0<=reader->ReadBytes(stream, code->code_length, code_bytes)){
		error("Code Attribute", reader->Position(stream));
		return NULL;
	}

	if(0<=reader->ReadUint16(stream, &code->exception_table_length)){
		error("Code Attribute", reader->Position(stream));
		return NULL;
	}
	uint32_t exception_table_size = sizeof(ExceptionInfo)*code->exception_table_length;
	code->exception_table = (ExceptionInfo*)GC_malloc(exception_table_size);
	for(int i=0;i<code->exception_table_length;i++){
		ExceptionInfo *ex = &code->exception_table[i];
		if(0<=reader->ReadUint16(stream, &ex->start_pc)){
			error("Code Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &ex->end_pc)){
			error("Code Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &ex->hanfler_pc)){
			error("Code Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &ex->catch_type)){
			error("Code Attribute", reader->Position(stream));
			return NULL;
		}
	}
	if(0<=reader->ReadUint16(stream, &code->attributes_count)){
		error("ConstantValue Attribute", reader->Position(stream));
		return NULL;
	}
	code->attributes = parseAttributes(stream, classfile, code->attributes_count);
	if(NULL==code->attributes){
		error("ConstantValue Attribute", reader->Position(stream));
		return NULL;
	}
	return code;
}

static Attribute_StackMapTable* attr_parse_stackMapTable(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_StackMapTable* tbl = (Attribute_StackMapTable*) GC_malloc(sizeof(Attribute_StackMapTable));
	tbl->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &tbl->attribute_length)){
		error("StackMapTable Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &tbl->entries_count)){
		error("StackMapTable Attribute", reader->Position(stream));
		return NULL;
	}
	if(!reader->Skip(stream,tbl->attribute_length)){
		error("StackMapTable Attribute", reader->Position(stream));
		return NULL;
	}
	return tbl;
}

static Attribute_Exceptions* attr_parse_exceptions(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_Exceptions* exceptions = (Attribute_Exceptions*) GC_malloc(sizeof(Attribute_Exceptions));
	exceptions->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &exceptions->attribute_length)){
		error("Exceptions Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &exceptions->exceptions_count)){
		error("Exceptions Attribute", reader->Position(stream));
		return NULL;
	}
	exceptions->exception_indexes = (uint16_t*)GC_malloc(sizeof(uint16_t)*exceptions->exceptions_count);
	for(int i=0;i<exceptions->exceptions_count;i++){
		if(0<=reader->ReadUint16(stream, &exceptions->exception_indexes[i])){
			error("Exceptions Attribute", reader->Position(stream));
			return NULL;
		}
	}
	return exceptions;
}

static Attribute_InnerClasses* attr_parse_innerClasses(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_InnerClasses* attr = (Attribute_InnerClasses*) GC_malloc(sizeof(Attribute_InnerClasses));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("InnerClasses Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->classes_count)){
		error("InnerClasses Attribute", reader->Position(stream));
		return NULL;
	}
	attr->classes = (InnerClassInfo*)GC_malloc(sizeof(InnerClassInfo)*attr->classes_count);
	for(int i=0;i<attr->classes_count;i++){
		if(0<=reader->ReadUint16(stream, &attr->classes[i].inner_class_info_index)){
			error("InnerClasses Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->classes[i].outer_class_info_index)){
			error("InnerClasses Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->classes[i].inner_name_index)){
			error("InnerClasses Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->classes[i].inner_class_access_flags)){
			error("InnerClasses Attribute", reader->Position(stream));
			return NULL;
		}
	} 
	return attr;
}

static Attribute_EnclosingMethod* attr_parse_enclosingMethod(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_EnclosingMethod* attr = (Attribute_EnclosingMethod*) GC_malloc(sizeof(Attribute_EnclosingMethod));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("EnclosingMethod Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->class_index)){
		error("EnclosingMethod Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->method_index)){
		error("EnclosingMethod Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static Attribute_Synthetic* attr_parse_synthetic(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_Synthetic* attr = (Attribute_Synthetic*) GC_malloc(sizeof(Attribute_Synthetic));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)||0!=attr->attribute_length){
		error("Synthetic Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static Attribute_Signature* attr_parse_signature(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_Signature* attr = (Attribute_Signature*) GC_malloc(sizeof(Attribute_Signature));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)||2!=attr->attribute_length){
		error("Signature Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->signature_index)){
		error("Signature Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static Attribute_SourceFile* attr_parse_sourceFile(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_SourceFile* attr = (Attribute_SourceFile*) GC_malloc(sizeof(Attribute_SourceFile));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)||2!=attr->attribute_length){
		error("SourceFile Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->sourcefile_index)){
		error("SourceFile Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static Attribute_SourceDebugExtension* attr_parse_sourceDebugExtention(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_SourceDebugExtension* attr = (Attribute_SourceDebugExtension*) GC_malloc(sizeof(Attribute_SourceDebugExtension));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)||2!=attr->attribute_length){
		error("SourceDebugExtention Attribute", reader->Position(stream));
		return NULL;
	}
	attr->debug_extension = (uint8_t*)GC_malloc(sizeof(uint8_t)*attr->attribute_length);
	if(0<=reader->ReadBytes(stream, attr->attribute_length, attr->debug_extension)){
		error("SourceDebugExtention Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static Attribute_LineNumberTable* attr_parse_lineNumberTable(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_LineNumberTable* attr = (Attribute_LineNumberTable*) GC_malloc(sizeof(Attribute_LineNumberTable));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("LineNumberTable Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->line_number_entries_count)){
		error("LineNumberTable Attribute", reader->Position(stream));
		return NULL;
	}
	attr->table = (LineNumberTableEntry*)GC_malloc(sizeof(LineNumberTableEntry)*attr->line_number_entries_count);
	for(int i=1;i<attr->line_number_entries_count;i++){
		if(0<=reader->ReadUint16(stream, &attr->table[i].start_pc)){
			error("LineNumberTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].line_number)){
			error("LineNumberTable Attribute", reader->Position(stream));
			return NULL;
		}

	}
	return attr;
}

static Attribute_LocalVariableTable* attr_parse_localVariableTable(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_LocalVariableTable* attr = (Attribute_LocalVariableTable*) GC_malloc(sizeof(Attribute_LocalVariableTable));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("LocalVariableTable Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->local_variable_entries_count)){
		error("LocalVariableTable Attribute", reader->Position(stream));
		return NULL;
	}
	attr->table = (LocalVariableTableEntry*)GC_malloc(sizeof(LocalVariableTableEntry)*attr->local_variable_entries_count);
	for(int i=1;i<attr->local_variable_entries_count;i++){
		if(0<=reader->ReadUint16(stream, &attr->table[i].start_pc)){
			error("LocalVariableTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].length)){
			error("LocalVariableTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].name_index)){
			error("LocalVariableTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].descriptor_index)){
			error("LocalVariableTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].index)){
			error("LocalVariableTable Attribute", reader->Position(stream));
			return NULL;
		}

	}
	return attr;
}

static Attribute_LocalVariableTypeTable* attr_parse_localVariableTypeTable(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_LocalVariableTypeTable* attr = (Attribute_LocalVariableTypeTable*) GC_malloc(sizeof(Attribute_LocalVariableTypeTable));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("LocalVariableTypeTable Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->local_variable_type_entries_count)){
		error("LocalVariableTypeTable Attribute", reader->Position(stream));
		return NULL;
	}
	attr->table = (LocalVariableTypeTableEntry*)GC_malloc(sizeof(LocalVariableTypeTableEntry)*attr->local_variable_type_entries_count);
	for(int i=1;i<attr->local_variable_type_entries_count;i++){
		if(0<=reader->ReadUint16(stream, &attr->table[i].start_pc)){
			error("LocalVariableTypeTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].length)){
			error("LocalVariableTypeTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].name_index)){
			error("LocalVariableTypeTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].signature_index)){
			error("LocalVariableTypeTable Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &attr->table[i].index)){
			error("LocalVariableTypeTable Attribute", reader->Position(stream));
			return NULL;
		}

	}
	return attr;
}

static Attribute_Deprecated* attr_parse_deprecated(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_Deprecated* attr = (Attribute_Deprecated*) GC_malloc(sizeof(Attribute_Deprecated));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)||0!=attr->attribute_length){
		error("Deprecated Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static ElementValue* parseElementValue(Stream *stream, ClassFile *classfile, ElementValue *value){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	if(0<=reader->ReadUint8(stream, &value->tag)){
		error("ElementValue", reader->Position(stream));
		return NULL;
	}
	switch(value->tag){
		case 'B': 
		case 'C': 
		case 'D': 
		case 'F':
		case 'I': 
		case 'J': 
		case 'S': 
		case 'Z': 
		case 's':
			if(0<=reader->ReadUint16(stream, &value->value.const_value_index)){
				error("ElementValue", reader->Position(stream));
				return NULL;
			}            
			break;
		case 'e': 
			if(0<=reader->ReadUint16(stream, &value->value.enum_const_value.type_name_index)){
				error("ElementValue", reader->Position(stream));
				return NULL;
			}            
			if(0<=reader->ReadUint16(stream, &value->value.enum_const_value.constant_name_index)){
				error("ElementValue", reader->Position(stream));
				return NULL;
			}            
			break;
		case 'c': 
			if(0<=reader->ReadUint16(stream, &value->value.class_info_index)){
				error("ElementValue", reader->Position(stream));
				return NULL;
			}            
			break;
		case '@': 
			if(NULL==parseAnnotation(stream, classfile, &value->value.annotation_value)){
				error("ElementValue", reader->Position(stream));
				return NULL;
			}
			break;
		case '[': 
			if(0<=reader->ReadUint16(stream, &value->value.array_value.values_count)){
				error("Deprecated Attribute", reader->Position(stream));
				return NULL;
			}
			value->value.array_value.values = (ElementValue*)GC_malloc(sizeof(ElementValue)*value->value.array_value.values_count);
			for(int i=0;i<value->value.array_value.values_count;i++){
				if(NULL==parseElementValue(stream, classfile, &value->value.array_value.values[i])){
					error("ElementValue", reader->Position(stream));
					return NULL;
				}
			}
			break;
		default:
			error("ElementValue", reader->Position(stream));
			return NULL;
	}
	return value;
}

static Annotation* parseAnnotation(Stream *stream, ClassFile *classfile, Annotation *annotation){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	if(0<=reader->ReadUint16(stream, &annotation->type_index)){
		error("Annotation", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &annotation->element_value_pairs_count)){
		error("Annotation", reader->Position(stream));
		return NULL;
	}
	annotation->pairs = (ElementValuePair*)GC_malloc(sizeof(ElementValuePair)*annotation->element_value_pairs_count);
	for(int j=0;j<annotation->element_value_pairs_count;j++){
		ElementValuePair *p = &(annotation->pairs[j]);
		if(0<=reader->ReadUint16(stream, &p->element_name_index)){
			error("Annotation", reader->Position(stream));
			return NULL;
		}
		if(NULL==parseElementValue(stream, classfile, &p->value)){
			error("Annotation", reader->Position(stream));
			return NULL;
		}
	}
	return annotation;
}

static Attribute_RuntimeVisibleAnnotations* attr_parse_runtimeVisibleAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_RuntimeVisibleAnnotations* attr = (Attribute_RuntimeVisibleAnnotations*) GC_malloc(sizeof(Attribute_RuntimeVisibleAnnotations));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("RuntimeVisibleAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->annotations_count)){
		error("RuntimeVisibleAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	attr->annotations = (Annotation*)GC_malloc(sizeof(Annotation)*attr->annotations_count); 
	for(int i=0;i<attr->annotations_count;i++){
		Annotation *annotation = &attr->annotations[i];
		if(NULL==parseAnnotation(stream, classfile, annotation)){
			error("RuntimeVisibleAnnotations", reader->Position(stream));
			return NULL;
		}
	}
	return attr;
}

static Attribute_RuntimeInvisibleAnnotations* attr_parse_runtimeInvisibleAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_RuntimeInvisibleAnnotations* attr = (Attribute_RuntimeInvisibleAnnotations*) GC_malloc(sizeof(Attribute_RuntimeInvisibleAnnotations));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("RuntimeInvisibleAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->annotations_count)){
		error("RuntimeInvisibleAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	attr->annotations = (Annotation*)GC_malloc(sizeof(Annotation)*attr->annotations_count); 
	for(int i=0;i<attr->annotations_count;i++){
		Annotation *annotation = &attr->annotations[i];
		if(NULL==parseAnnotation(stream, classfile, annotation)){
			error("RuntimeInvisibleAnnotations", reader->Position(stream));
			return NULL;
		}
	}
	return attr;
}

static Attribute_RuntimeVisibleParameterAnnotations* attr_parse_runtimeVisibleParameterAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_RuntimeVisibleParameterAnnotations* attr = (Attribute_RuntimeVisibleParameterAnnotations*) GC_malloc(sizeof(Attribute_RuntimeVisibleParameterAnnotations));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("RuntimeVisibleParameterAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint8(stream, &attr->parameters_count)){
		error("RuntimeVisibleParameterAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	attr->parameter_annotations = (ParameterAnnotaion*)GC_malloc(sizeof(ParameterAnnotaion)*attr->parameters_count); 
	for(int i=0;i<attr->parameters_count;i++){
		ParameterAnnotaion *parameter_annotation = &attr->parameter_annotations[i];
		if(0<=reader->ReadUint16(stream, &parameter_annotation->annotations_count)){
			error("RuntimeVisibleParameterAnnotations Attribute", reader->Position(stream));
			return NULL;
		}
		parameter_annotation->annotations = (Annotation*)GC_malloc(sizeof(Annotation)*parameter_annotation->annotations_count);
		for(int j=0;j<parameter_annotation->annotations_count;j++){
			if(NULL==parseAnnotation(stream, classfile, &parameter_annotation->annotations[j])){
				error("RuntimeVisibleParameterAnnotations Attribute", reader->Position(stream));
				return NULL;
			}
		}
	}
	return attr;
}

static Attribute_RuntimeInvisibleParameterAnnotations* attr_parse_runtimeInvisibleParameterAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_RuntimeInvisibleParameterAnnotations* attr = (Attribute_RuntimeInvisibleParameterAnnotations*) GC_malloc(sizeof(Attribute_RuntimeInvisibleParameterAnnotations));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("RuntimeInvisibleParameterAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint8(stream, &attr->parameters_count)){
		error("RuntimeInvisibleParameterAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	attr->parameter_annotations = (ParameterAnnotaion*)GC_malloc(sizeof(ParameterAnnotaion)*attr->parameters_count); 
	for(int i=0;i<attr->parameters_count;i++){
		ParameterAnnotaion *parameter_annotation = &attr->parameter_annotations[i];
		if(0<=reader->ReadUint16(stream, &parameter_annotation->annotations_count)){
			error("RuntimeInvisibleParameterAnnotations Attribute", reader->Position(stream));
			return NULL;
		}
		parameter_annotation->annotations = (Annotation*)GC_malloc(sizeof(Annotation)*parameter_annotation->annotations_count);
		for(int j=0;j<parameter_annotation->annotations_count;j++){
			if(NULL==parseAnnotation(stream, classfile, &parameter_annotation->annotations[j])){
				error("RuntimeInvisibleParameterAnnotations Attribute", reader->Position(stream));
				return NULL;
			}
		}
	}
	return attr;
}

static TypeAnnotation* parseTypeAnnotation(Stream *stream, ClassFile *classfile, TypeAnnotation *annotation){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	if(0<=reader->ReadUint8(stream, &annotation->target_type)){
		error("TypeAnnotation Attribute", reader->Position(stream));
		return NULL;
	}

	switch(annotation->target_type){
		case 0x00:
		case 0x01:
			if(0<=reader->ReadUint8(stream, &annotation->target_info.type_parameter_target.type_parameter_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x10:
			if(0<=reader->ReadUint16(stream, &annotation->target_info.super_type_target.supertype_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x11:
		case 0x12:
			if(0<=reader->ReadUint8(stream, &annotation->target_info.type_parameter_bound_target.type_parameter_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			if(0<=reader->ReadUint8(stream, &annotation->target_info.type_parameter_bound_target.bound_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x13:
		case 0x14:
		case 0x15:
			// EmptyTarget
			break;
		case 0x16:
			if(0<=reader->ReadUint8(stream, &annotation->target_info.formal_parameter_target.formal_parameter_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x17:
			if(0<=reader->ReadUint16(stream, &annotation->target_info.throws_target.throws_type_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x40:
		case 0x41:
			if(0<=reader->ReadUint16(stream, &annotation->target_info.localvar_target.count)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			annotation->target_info.localvar_target.elements = (LocalvarElement*)GC_malloc(sizeof(LocalvarElement)*annotation->target_info.localvar_target.count);
			for(int i=0;i<annotation->target_info.localvar_target.count;i++){
				LocalvarElement *element = &annotation->target_info.localvar_target.elements[i];
				if(0<=reader->ReadUint16(stream, &element->start_pc)){
					error("TypeAnnotation Attribute", reader->Position(stream));
					return NULL;
				}
				if(0<=reader->ReadUint16(stream, &element->length)){
					error("TypeAnnotation Attribute", reader->Position(stream));
					return NULL;
				}
				if(0<=reader->ReadUint16(stream, &element->index)){
					error("TypeAnnotation Attribute", reader->Position(stream));
					return NULL;
				}

			}
			break;
		case 0x42:
			if(0<=reader->ReadUint16(stream, &annotation->target_info.catch_target.exception_table_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
			if(0<=reader->ReadUint16(stream, &annotation->target_info.offset_target.offset)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B:
			if(0<=reader->ReadUint16(stream, &annotation->target_info.type_arugument_target.offset)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			if(0<=reader->ReadUint8(stream, &annotation->target_info.type_arugument_target.type_arugument_index)){
				error("TypeAnnotation Attribute", reader->Position(stream));
				return NULL;
			}
			break;
		default:
			break;
	}

	if(0<=reader->ReadUint8(stream, &annotation->target_path.path_count)){
		error("TypeAnnotation Attribute", reader->Position(stream));
		return NULL;
	}
	annotation->target_path.path = (_Path*)GC_malloc(sizeof(_Path)*annotation->target_path.path_count);
	for(int i=0; i<annotation->target_path.path_count; i++){
		_Path *p = &annotation->target_path.path[i]; 
		if(0<=reader->ReadUint8(stream, &p->typepath_kind)){
			error("TypeAnnotation Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint8(stream, &p->type_arugument_index)){
			error("TypeAnnotation Attribute", reader->Position(stream));
			return NULL;
		}
	}

	if(0<=reader->ReadUint16(stream, &annotation->type_index)){
		error("TypeAnnotation Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &annotation->element_value_pairs_count)){
		error("TypeAnnotation Attribute", reader->Position(stream));
		return NULL;
	}

	annotation->pairs = (ElementValuePair*)GC_malloc(sizeof(ElementValuePair)*annotation->element_value_pairs_count);
	for(int j=0;j<annotation->element_value_pairs_count;j++){
		ElementValuePair *p = &annotation->pairs[j];
		if(0<=reader->ReadUint16(stream, &p->element_name_index)){
			error("Annotation", reader->Position(stream));
			return NULL;
		}
		if(NULL==parseElementValue(stream, classfile, &p->value)){
			error("TypeAnnotation Attribute", reader->Position(stream));
			return NULL;
		}
	}   

}

static Attribute_RuntimeVisibleTypeAnnotations* attr_parse_runtimeVisibleTypeAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_RuntimeVisibleTypeAnnotations* attr = (Attribute_RuntimeVisibleTypeAnnotations*) GC_malloc(sizeof(Attribute_RuntimeVisibleTypeAnnotations));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("RuntimeVisibleTypeAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->annotations_count)){
		error("RuntimeVisibleTypeAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	attr->annotations = (TypeAnnotation*)GC_malloc(sizeof(TypeAnnotation)*attr->annotations_count); 
	for(int i=0;i<attr->annotations_count;i++){
		if(NULL==parseTypeAnnotation(stream,classfile, &attr->annotations[i])){
			error("RuntimeVisibleParameterAnnotations Attribute", reader->Position(stream));
			return NULL;
		}
	}
	return attr;
}

static Attribute_RuntimeInvisibleTypeAnnotations* attr_parse_runtimeInvisibleTypeAnnotations(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_RuntimeInvisibleTypeAnnotations* attr = (Attribute_RuntimeInvisibleTypeAnnotations*) GC_malloc(sizeof(Attribute_RuntimeInvisibleTypeAnnotations));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("RuntimeInvisibleTypeAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->annotations_count)){
		error("RuntimeInvisibleTypeAnnotations Attribute", reader->Position(stream));
		return NULL;
	}
	attr->annotations = (TypeAnnotation*)GC_malloc(sizeof(TypeAnnotation)*attr->annotations_count); 
	for(int i=0;i<attr->annotations_count;i++){
		if(NULL==parseTypeAnnotation(stream, classfile, &attr->annotations[i])){
			error("RuntimeInvisibleParameterAnnotations Attribute", reader->Position(stream));
			return NULL;
		}
	}
	return attr;
}

static Attribute_AnnotationDefault* attr_parse_annotationDefault(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_AnnotationDefault* attr = (Attribute_AnnotationDefault*) GC_malloc(sizeof(Attribute_AnnotationDefault));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("AnnotationDefault Attribute", reader->Position(stream));
		return NULL;
	}
	if(NULL==parseElementValue(stream, classfile, &attr->default_value)){
		error("AnnotationDefault Attribute", reader->Position(stream));
		return NULL;
	}
	return attr;
}

static Attribute_BootstrapMethods* attr_parse_bootstrapMethods(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_BootstrapMethods* attr = (Attribute_BootstrapMethods*) GC_malloc(sizeof(Attribute_BootstrapMethods));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("BootstrapMethod Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint16(stream, &attr->bootstrap_methods_count)){
		error("BootstrapMethod Attribute", reader->Position(stream));
		return NULL;
	}
	attr->bootstrap_methods = (BootstrapMethod*)GC_malloc(sizeof(BootstrapMethod)*attr->bootstrap_methods_count);
	for(int i=0;i<attr->bootstrap_methods_count;i++){
		BootstrapMethod *m = &attr->bootstrap_methods[i];
		if(0<=reader->ReadUint16(stream, &m->bootstrap_method_ref)){
			error("BootstrapMethod Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &m->arguments_count)){
			error("BootstrapMethod Attribute", reader->Position(stream));
			return NULL;
		}
		m->arguments = (uint16_t*)GC_malloc(sizeof(uint16_t)*m->arguments_count);
		for(int j=0;j<m->arguments_count;j++){
			if(0<=reader->ReadUint16(stream, &m->arguments[j])){
				error("BootstrapMethod Attribute", reader->Position(stream));
				return NULL;
			}
		}
	}
	return attr;
}

static Attribute_MethodParameters* attr_parse_methodParameters(Stream *stream, ClassFile *classfile, uint16_t name_index){
	StreamReaderOp *reader = (StreamReaderOp*)stream->reader;
	Attribute_MethodParameters* attr = (Attribute_MethodParameters*) GC_malloc(sizeof(Attribute_MethodParameters));
	attr->attribute_name_index = name_index;
	if(0<=reader->ReadUint32(stream, &attr->attribute_length)){
		error("MethodParameters Attribute", reader->Position(stream));
		return NULL;
	}
	if(0<=reader->ReadUint8(stream, &attr->parameters_count)){
		error("BootstrapMethod Attribute", reader->Position(stream));
		return NULL;
	}
	attr->parameters= (_Parameter*)GC_malloc(sizeof(_Parameter)*attr->parameters_count);
	for(int i=0;i<attr->parameters_count;i++){
		_Parameter *p = &attr->parameters[i];
		if(0<=reader->ReadUint16(stream, &p->name_index)){
			error("MethodParameters Attribute", reader->Position(stream));
			return NULL;
		}
		if(0<=reader->ReadUint16(stream, &p->access_flags)){
			error("MethodParameters Attribute", reader->Position(stream));
			return NULL;
		}
	}
	return attr;
}
