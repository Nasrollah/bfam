#include "occaSerial.hpp"

namespace occa {
  //---[ Helper Functions ]-----------
  namespace cpu {
    std::string getFieldFrom(const std::string &command,
                             const std::string &field){
#if (OCCA_OS & LINUX_OS)
      std::stringstream ss;

      ss << "echo \"(. " << getOCCADir() << "/scripts/shellTools.sh; " << command << " '" << field << "')\" | bash";

      std::string sCommand = ss.str();

      FILE *fp;
      fp = popen(sCommand.c_str(), "r");

      const int bufferSize = 4096;
      char *buffer = new char[bufferSize];

      ignoreResult( fread(buffer, sizeof(char), bufferSize, fp) );

      pclose(fp);

      int end;

      for(end = 0; end < bufferSize; ++end){
        if(buffer[end] == '\n')
          break;
      }

      std::string ret(buffer, end);

      delete [] buffer;

      return ret;
#else
      return "";
#endif
    }

    std::string getProcessorName(){
#if   (OCCA_OS & LINUX_OS)
      return getFieldFrom("getCPUINFOField", "model name");
#elif (OCCA_OS == OSX_OS)
      size_t bufferSize = 100;
      char buffer[100];

      sysctlbyname("machdep.cpu.brand_string",
                   &buffer, &bufferSize,
                   NULL, 0);

      return std::string(buffer);
#elif (OCCA_OS == WINDOWS_OS)
      char buffer[MAX_COMPUTERNAME_LENGTH + 1];
      int bytes;

      GetComputerName((LPWSTR) buffer, (LPDWORD) &bytes);

      return std::string(buffer, bytes);
#endif
    }

    int getCoreCount(){
#if (OCCA_OS & (LINUX_OS | OSX_OS))
      return sysconf(_SC_NPROCESSORS_ONLN);
#elif (OCCA_OS == WINDOWS_OS)
      SYSTEM_INFO sysinfo;
      GetSystemInfo(&sysinfo);
      return sysinfo.dwNumberOfProcessors;
#endif
    }

    int getProcessorFrequency(){
#if   (OCCA_OS & LINUX_OS)
      std::stringstream ss;
      int freq;

      ss << getFieldFrom("getCPUINFOField", "cpu MHz");

      ss >> freq;

      return freq;
#elif (OCCA_OS == OSX_OS)
      uint64_t frequency = 0;
      size_t size = sizeof(frequency);

      int error = sysctlbyname("hw.cpufrequency", &frequency, &size, NULL, 0);

      OCCA_CHECK(error != ENOMEM,
                 "Error getting CPU Frequency.\n");

      return frequency/1.0e6;
#elif (OCCA_OS == WINDOWS_OS)
      LARGE_INTEGER performanceFrequency;
      QueryPerformanceFrequency(&performanceFrequency);

      return (int) (((double) performanceFrequency.QuadPart) / 1000.0);
#endif
    }

    std::string getProcessorCacheSize(int level){
#if   (OCCA_OS & LINUX_OS)
      std::stringstream field;

      field << 'L' << level;

      if(level == 1)
        field << 'd';

      field << " cache";

      return getFieldFrom("getLSCPUField", field.str());
#elif (OCCA_OS == OSX_OS)
      std::stringstream ss;
      ss << "hw.l" << level;

      if(level == 1)
        ss << 'd';

      ss << "cachesize";

      std::string field = ss.str();

      uint64_t cache = 0;
      size_t size = sizeof(cache);

      int error = sysctlbyname(field.c_str(), &cache, &size, NULL, 0);

      OCCA_CHECK(error != ENOMEM,
                 "Error getting L" << level << " Cache Size.\n");

      ss.str("");

      if(cache < (1 << 10))
        ss << cache << " B";
      else if(cache < (1 << 20))
        ss << (cache >> 10) << " KB";
      else if(cache < (1 << 30))
        ss << (cache >> 20) << " MB";

      return ss.str();
#elif (OCCA_OS == WINDOWS_OS)
      std::stringstream ss;
      DWORD cache = 0;

      PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
      int bytes;

      GetLogicalProcessorInformation(buffer, (LPDWORD) &bytes);

      OCCA_CHECK((GetLastError() == ERROR_INSUFFICIENT_BUFFER),
                 "[GetLogicalProcessorInformation] Failed");

      buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) cpu::malloc(bytes);

      bool passed = GetLogicalProcessorInformation(buffer, (LPDWORD) &bytes);

      OCCA_CHECK(passed,
                 "[GetLogicalProcessorInformation] Failed");

      PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pos = buffer;
      int off = 0;

      while((off + sizeof(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)) <= bytes){
        switch(pos->Relationship){
        case RelationCache:{
          CACHE_DESCRIPTOR info = pos->Cache;

          if (info.Level == level)
            cache = info.Size;
        }
        }

        if(cache)
          break;
      }

      if(cache < (1 << 10))
        ss << cache << " B";
      else if(cache < (1 << 20))
        ss << (cache >> 10) << " KB";
      else if(cache < (1 << 30))
        ss << (cache >> 20) << " MB";

      cpu::free(buffer);

      return ss.str();
#endif
    }

    std::string getDeviceListInfo(){
      std::stringstream ss, ssFreq;

      ss << getCoreCount();

      const int freq = getProcessorFrequency();

      if(freq < 1000)
        ssFreq << freq << " MHz";
      else
        ssFreq << (freq/1000.0) << " GHz";

      std::string l1 = getProcessorCacheSize(1);
      std::string l2 = getProcessorCacheSize(2);
      std::string l3 = getProcessorCacheSize(3);

      size_t maxSize = ((l1.size() < l2.size()) ? l2.size() : l1.size());
      maxSize        = ((maxSize   < l3.size()) ? l3.size() : maxSize  );

      if(maxSize){
        l1 = std::string(maxSize - l1.size(), ' ') + l1;
        l2 = std::string(maxSize - l2.size(), ' ') + l2;
        l3 = std::string(maxSize - l3.size(), ' ') + l3;
      }

      std::string tab[2];
      tab[0] = "   CPU Info   ";
      tab[1] = "              ";

      std::string processorName  = getProcessorName();
      std::string clockFrequency = ssFreq.str();
      std::string coreCount      = ss.str();

      ss.str("");

      bool ps = false;

      // << "==============o=======================o==========================================\n";
      if(processorName.size())
        ss << tab[ps]  << "|  Processor Name       | " << processorName                   << '\n'; ps = true;
      if(coreCount.size())
        ss << tab[ps]  << "|  Cores                | " << coreCount                       << '\n'; ps = true;
      if(clockFrequency.size())
        ss << tab[ps]  << "|  Clock Frequency      | " << clockFrequency                  << '\n'; ps = true;
      ss   << tab[ps]  << "|  SIMD Instruction Set | " << OCCA_VECTOR_SET                 << '\n'
           << tab[ps]  << "|  SIMD Width           | " << (32*OCCA_SIMD_WIDTH) << " bits" << '\n'; ps = true;
      if(l1.size())
        ss << tab[ps]  << "|  L1 Cache Size (d)    | " << l1                              << '\n';
      if(l2.size())
        ss << tab[ps]  << "|  L2 Cache Size        | " << l2                              << '\n';
      if(l3.size())
        ss << tab[ps]  << "|  L3 Cache Size        | " << l3                              << '\n';
      // << "==============o=======================o==========================================\n";

      return ss.str();
    }

    int compilerVendor(const std::string &compiler){
#if (OCCA_OS & (LINUX_OS | OSX_OS))
      if((compiler.find("gcc") != std::string::npos) ||
         (compiler.find("g++") != std::string::npos)){

        return cpu::vendor::GNU;
      }
      else if((compiler.find("clang")   != std::string::npos) ||
              (compiler.find("clang++") != std::string::npos)){

        return cpu::vendor::LLVM;
      }
      else if((compiler.find("icc")  != std::string::npos) ||
              (compiler.find("icpc") != std::string::npos)){

        return cpu::vendor::Intel;
      }
      else if(compiler.find("pathCC") != std::string::npos){
        return cpu::vendor::Pathscale;
      }
      else if((compiler.find("xlc")   != std::string::npos) ||
              (compiler.find("xlc++") != std::string::npos)){

        return cpu::vendor::IBM;
      }
      else if((compiler.find("pgcc")  != std::string::npos) ||
              (compiler.find("pgc++") != std::string::npos)){

        return cpu::vendor::PGI;
      }
      else if(compiler.find("aCC") != std::string::npos){
        return cpu::vendor::HP;
      }
      else if((compiler.find("cc") != std::string::npos) ||
              (compiler.find("CC") != std::string::npos)){

        return cpu::vendor::Cray;
      }

      // Find the compiler manually
      std::stringstream ss;
      int vendor_ = cpu::vendor::notFound;

      const std::string safeCompiler = removeSlashes(compiler);
      const std::string cacheDir     = getCachePath();

      const std::string testFilename = (cacheDir + std::string("/.compilerVendorTest.cpp"));
      const std::string infoFilename = (cacheDir + std::string("/.compilerVendorTest_") + safeCompiler);

      if(!haveFile(testFilename)){
        waitForFile(testFilename);
      }
      else{
        if(!fileExists(testFilename)){
          ss << "int main(int argc, char **argv){\n"
             << "#if defined(__clang__)\n"
             << "  return " << cpu::vendor::b_LLVM << ";\n"
             << "\n"
             << "#elif defined(__ICC) || defined(__INTEL_COMPILER)\n"
             << "  return " << cpu::vendor::b_Intel << ";\n"
             << "\n"
             << "#elif defined(__GNUC__) || defined(__GNUG__)\n"
             << "  return " << cpu::vendor::b_GNU << ";\n"
             << "\n"
             << "#elif defined(__HP_cc) || defined(__HP_aCC)\n"
             << "  return " << cpu::vendor::b_HP << ";\n"
             << "\n"
             << "#elif defined(__IBMC__) || defined(__IBMCPP__)\n"
             << "  return " << cpu::vendor::b_IBM << ";\n"
             << "\n"
             << "#elif defined(__PGI)\n"
             << "  return " << cpu::vendor::b_PGI << ";\n"
             << "\n"
             << "#elif defined(_CRAYC)\n"
             << "  return " << cpu::vendor::b_Cray << ";\n"
             << "\n"
             << "#elif defined(__PATHSCALE__) || defined(__PATHCC__)\n"
             << "  return " << cpu::vendor::b_Pathscale << ";\n"
             << "#endif\n"
             << "\n"
             << "  // Missing\n"
             << "  return " << cpu::vendor::b_max << ";\n"
             << "}\n";

          writeToFile(testFilename, ss.str());
          ss.str("");
        }

        const std::string binaryFilename = (cacheDir + std::string("/.compilerVendorBinary_") + safeCompiler);

        if(!fileExists(infoFilename)){
          ss << compiler
             << ' '
             << testFilename
             << " -o "
             << binaryFilename
             << " > /dev/null 2>&1";

          const int compileError = system(ss.str().c_str());

          if(!compileError){
            int exitStatus = system(binaryFilename.c_str());
            int vendorBit  = WEXITSTATUS(exitStatus);

            if(vendorBit < cpu::vendor::b_max)
              vendor_ = (1 << vendorBit);
          }

          ss.str("");
          ss << vendor_;

          writeToFile(infoFilename, ss.str());

          releaseFile(testFilename);

          return vendor_;
        }
        else
          releaseFile(testFilename);
      }

      ss << readFile(infoFilename);
      ss >> vendor_;

      return vendor_;

#elif (OCCA_OS == WINDOWS_OS)
#  if OCCA_USING_VS
      return cpu::vendor::VisualStudio;
#  endif

      if(compiler.find("cl.exe") != std::string::npos){
        return cpu::vendor::VisualStudio;
      }
#endif

      return cpu::vendor::notFound;
    }

    std::string compilerSharedBinaryFlags(const std::string &compiler){
      return compilerSharedBinaryFlags( cpu::compilerVendor(compiler) );
    }

    std::string compilerSharedBinaryFlags(const int vendor_){
      if(vendor_ & (cpu::vendor::GNU   |
                    cpu::vendor::LLVM  |
                    cpu::vendor::Intel |
                    cpu::vendor::IBM   |
                    cpu::vendor::PGI   |
                    cpu::vendor::Cray  |
                    cpu::vendor::Pathscale)){

        return "-x c++ -fPIC -shared"; // [-] -x c++ for now
      }
      else if(vendor_ & cpu::vendor::HP){
        return "+z -b";
      }
      else if(vendor_ & cpu::vendor::VisualStudio){
#if OCCA_DEBUG_ENABLED
        return "/TP /LD /MDd";
#else
        return "/TP /LD /MD";
#endif
      }

      return "";
    }

    void addSharedBinaryFlagsTo(const std::string &compiler, std::string &flags){
      addSharedBinaryFlagsTo(cpu::compilerVendor(compiler), flags);
    }

    void addSharedBinaryFlagsTo(const int vendor_, std::string &flags){
      std::string sFlags = cpu::compilerSharedBinaryFlags(vendor_);

      if(flags.size() == 0)
        flags = sFlags;

      if(flags.find(sFlags) == std::string::npos)
        flags = (sFlags + " " + flags);
    }

    void* malloc(uintptr_t bytes){
      void* ptr;

#if   (OCCA_OS & LINUX_OS)
      ignoreResult( posix_memalign(&ptr, OCCA_MEM_ALIGN, bytes) );
#elif (OCCA_OS == OSX_OS)
      ptr = ::malloc(bytes);
#elif (OCCA_OS == WINDOWS_OS)
      ptr = ::malloc(bytes);
#endif

      return ptr;
    }

    void free(void *ptr){
      ::free(ptr);
    }

    void* dlopen(const std::string &filename,
                 const bool releaseWithError){

#if (OCCA_OS & (LINUX_OS | OSX_OS))
      void *dlHandle = ::dlopen(filename.c_str(), RTLD_NOW);

      if((dlHandle == NULL) && releaseWithError){
        releaseFile(filename);
        OCCA_CHECK(false,
                   "Error loading binary [" << filename << "] with dlopen");
      }
#else
      void *dlHandle = LoadLibraryA(filename.c_str());

      if((dlHandle == NULL) && releaseWithError){
        releaseFile(filename);

        OCCA_CHECK(dlHandle != NULL,
                   "Error loading dll [" << filename << "] (WIN32 error: " << GetLastError() << ")");
      }
#endif

      return dlHandle;
    }

    void* dlsym(void *dlHandle,
                const std::string &filename,
                const std::string &functionName,
                const bool releaseWithError){

#if (OCCA_OS & (LINUX_OS | OSX_OS))
      void *sym = ::dlsym(dlHandle, functionName.c_str());

      char *dlError;

      if(((dlError = dlerror()) != NULL) && releaseWithError){
        releaseFile(filename);
        OCCA_CHECK(false,
                   "Error loading symbol from binary with dlsym (DL Error: " << dlError << ")");
      }
#else
      void *sym = GetProcAddress((HMODULE) dlHandle, functionName.c_str());

      if((sym == NULL) && releaseWithError){
        releaseFile(filename);
        OCCA_CHECK(false,
                   "Error loading symbol from binary with GetProcAddress");
      }
#endif

      return sym;
    }
  };
  //==================================


  //---[ Kernel ]---------------------
  template <>
  kernel_t<Serial>::kernel_t(){
    data    = NULL;
    dHandle = NULL;

    dims  = 1;
    inner = occa::dim(1,1,1);
    outer = occa::dim(1,1,1);

    nestedKernelCount = 0;
    nestedKernels     = NULL;

    startTime = (void*) new double;
    endTime   = (void*) new double;
  }

  template <>
  kernel_t<Serial>::kernel_t(const kernel_t<Serial> &k){
    data    = k.data;
    dHandle = k.dHandle;

    metaInfo = k.metaInfo;

    dims  = k.dims;
    inner = k.inner;
    outer = k.outer;

    nestedKernelCount = k.nestedKernelCount;
    nestedKernels     = k.nestedKernels;

    for(int i = 0; i < nestedKernelCount; ++i)
      nestedKernels[i] = k.nestedKernels[i];

    startTime = k.startTime;
    endTime   = k.endTime;
  }

  template <>
  kernel_t<Serial>& kernel_t<Serial>::operator = (const kernel_t<Serial> &k){
    data    = k.data;
    dHandle = k.dHandle;

    metaInfo = k.metaInfo;

    dims  = k.dims;
    inner = k.inner;
    outer = k.outer;

    nestedKernelCount = k.nestedKernelCount;
    nestedKernels     = k.nestedKernels;

    for(int i = 0; i < nestedKernelCount; ++i)
      nestedKernels[i] = k.nestedKernels[i];

    *((double*) startTime) = *((double*) k.startTime);
    *((double*) endTime)   = *((double*) k.endTime);

    return *this;
  }

  template <>
  kernel_t<Serial>::~kernel_t(){}

  template <>
  std::string kernel_t<Serial>::getCachedBinaryName(const std::string &filename,
                                                    kernelInfo &info_){

    std::string cachedBinary = getCachedName(filename,
                                             dHandle->getInfoSalt(info_));

#if (OCCA_OS & WINDOWS_OS)
    // Windows requires .dll extension
    cachedBinary = cachedBinary + ".dll";
#endif

    return cachedBinary;
  }

  template <>
  kernel_t<Serial>* kernel_t<Serial>::buildFromSource(const std::string &filename,
                                                      const std::string &functionName,
                                                      const kernelInfo &info_){
    kernelInfo info = info_;

    dHandle->addOccaHeadersToInfo(info);

    std::string cachedBinary = getCachedBinaryName(filename, info);

    if(!haveFile(cachedBinary)){
      waitForFile(cachedBinary);

      if(verboseCompilation_f)
        std::cout << "Found cached binary of [" << filename << "] in [" << cachedBinary << "]\n";

      return buildFromBinary(cachedBinary, functionName);
    }

    struct stat buffer;
    const bool fileExists = (stat(cachedBinary.c_str(), &buffer) == 0);

    if(fileExists){
      releaseFile(cachedBinary);

      if(verboseCompilation_f)
        std::cout << "Found cached binary of [" << filename << "] in [" << cachedBinary << "]\n";

      return buildFromBinary(cachedBinary, functionName);
    }

    data = new SerialKernelData_t;

    const std::string iCachedBinary = createIntermediateSource(filename,
                                                               cachedBinary,
                                                               info,
                                                               !useParser);

    const std::string occaDir = getOCCADir();

    std::stringstream command;

    if(dHandle->compilerEnvScript.size())
      command << dHandle->compilerEnvScript << " && ";

#if (OCCA_OS & (LINUX_OS | OSX_OS))
    command << dHandle->compiler
            << ' '    << dHandle->compilerFlags
            << ' '    << info.flags
            << ' '    << iCachedBinary
            << " -o " << cachedBinary
            << " -I"  << occaDir << "/include"
            << " -L"  << occaDir << "/lib -locca"
            << std::endl;
#else
#  if (OCCA_DEBUG_ENABLED)
    std::string occaLib = occaDir + "\\lib\\libocca_d.lib ";
#  else
    std::string occaLib = occaDir + "\\lib\\libocca.lib ";
#  endif
    std::string ptLib = occaDir + "\\lib\\pthreadVC2.lib ";

    command << dHandle->compiler
            << " /D MC_CL_EXE"
            << ' '    << dHandle->compilerFlags
            << ' '    << info.flags
            << " /I"  << occaDir << "\\include"     // NBN: /include
            << ' '    << iCachedBinary
            << " /link " << occaLib << ptLib << " /OUT:" << cachedBinary
            << std::endl;
#endif

    const std::string &sCommand = command.str();

    if(verboseCompilation_f)
      std::cout << "Compiling [" << functionName << "]\n" << sCommand << "\n";

#if (OCCA_OS & (LINUX_OS | OSX_OS))
    const int compileError = system(sCommand.c_str());
#else
    const int compileError = system(("\"" +  sCommand + "\"").c_str());
#endif

    if(compileError){
      releaseFile(cachedBinary);
      OCCA_CHECK(false, "Compilation error");
    }

    OCCA_EXTRACT_DATA(Serial, Kernel);

    data_.dlHandle = cpu::dlopen(cachedBinary, true);
    data_.handle   = cpu::dlsym(data_.dlHandle, cachedBinary, functionName, true);

    releaseFile(cachedBinary);

    return this;
  }

  template <>
  kernel_t<Serial>* kernel_t<Serial>::buildFromBinary(const std::string &filename,
                                                      const std::string &functionName){
    data = new SerialKernelData_t;

    OCCA_EXTRACT_DATA(Serial, Kernel);

    data_.dlHandle = cpu::dlopen(filename, false);
    data_.handle   = cpu::dlsym(data_.dlHandle, filename, functionName, false);

    return this;
  }

  template <>
  kernel_t<Serial>* kernel_t<Serial>::loadFromLibrary(const char *cache,
                                                      const std::string &functionName){
    return buildFromBinary(cache, functionName);
  }

  // [-] Missing
  template <>
  int kernel_t<Serial>::preferredDimSize(){
    preferredDimSize_ = OCCA_SIMD_WIDTH;
    return OCCA_SIMD_WIDTH;
  }

#include "operators/occaSerialKernelOperators.cpp"

  template <>
  double kernel_t<Serial>::timeTaken(){
    const double &start = *((double*) startTime);
    const double &end   = *((double*) endTime);

    return 1.0e3*(end - start);
  }

  template <>
  double kernel_t<Serial>::timeTakenBetween(void *start, void *end){
    const double &start_ = *((double*) start);
    const double &end_   = *((double*) end);

    return 1.0e3*(end_ - start_);
  }

  template <>
  void kernel_t<Serial>::free(){
    OCCA_EXTRACT_DATA(Serial, Kernel);

#if (OCCA_OS & (LINUX_OS | OSX_OS))
    dlclose(data_.dlHandle);
#else
    FreeLibrary((HMODULE) (data_.dlHandle));
#endif
  }
  //==================================


  //---[ Memory ]---------------------
  template <>
  memory_t<Serial>::memory_t(){
    handle    = NULL;
    mappedPtr = NULL;
    uvaPtr    = NULL;

    dHandle = NULL;
    size    = 0;

    isTexture = false;
    textureInfo.arg = NULL;
    textureInfo.dim = 1;
    textureInfo.w = textureInfo.h = textureInfo.d = 0;

    uva_inDevice = false;
    uva_isDirty  = false;

    isManaged  = false;
    isMapped   = false;
    isAWrapper = false;
  }

  template <>
  memory_t<Serial>::memory_t(const memory_t<Serial> &m){
    *this = m;
  }

  template <>
  memory_t<Serial>& memory_t<Serial>::operator = (const memory_t<Serial> &m){
    handle    = m.handle;
    mappedPtr = m.mappedPtr;
    uvaPtr    = m.uvaPtr;

    dHandle = m.dHandle;
    size    = m.size;

    isTexture = m.isTexture;
    textureInfo.arg  = m.textureInfo.arg;
    textureInfo.dim  = m.textureInfo.dim;

    textureInfo.w = m.textureInfo.w;
    textureInfo.h = m.textureInfo.h;
    textureInfo.d = m.textureInfo.d;

    if(isTexture)
      handle = &textureInfo;

    uva_inDevice = m.uva_inDevice;
    uva_isDirty  = m.uva_isDirty;

    isManaged  = m.isManaged;
    isMapped   = m.isMapped;
    isAWrapper = m.isAWrapper;

    return *this;
  }

  template <>
  memory_t<Serial>::~memory_t(){}

  template <>
  void* memory_t<Serial>::getMemoryHandle(){
    return handle;
  }

  template <>
  void* memory_t<Serial>::getTextureHandle(){
    return textureInfo.arg;
  }

  template <>
  void memory_t<Serial>::copyFrom(const void *src,
                                  const uintptr_t bytes,
                                  const uintptr_t offset){
    dHandle->finish();

    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << offset << " , " << (offset + bytes_) << " ]");

    void *destPtr      = ((char*) (isTexture ? textureInfo.arg : handle)) + offset;
    const void *srcPtr = src;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::copyFrom(const memory_v *src,
                                  const uintptr_t bytes,
                                  const uintptr_t destOffset,
                                  const uintptr_t srcOffset){
    dHandle->finish();

    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + destOffset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << destOffset << " , " << (destOffset + bytes_) << " ]");

    OCCA_CHECK((bytes_ + srcOffset) <= src->size,
               "Source has size [" << src->size << "],"
               << "trying to access [ " << srcOffset << " , " << (srcOffset + bytes_) << " ]");

    void *destPtr      = ((char*) (isTexture      ? textureInfo.arg      : handle))      + destOffset;
    const void *srcPtr = ((char*) (src->isTexture ? src->textureInfo.arg : src->handle)) + srcOffset;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::copyTo(void *dest,
                                const uintptr_t bytes,
                                const uintptr_t offset){
    dHandle->finish();

    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << offset << " , " << (offset + bytes_) << " ]");

    void *destPtr      = dest;
    const void *srcPtr = ((char*) (isTexture ? textureInfo.arg : handle)) + offset;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::copyTo(memory_v *dest,
                                const uintptr_t bytes,
                                const uintptr_t destOffset,
                                const uintptr_t srcOffset){
    dHandle->finish();

    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + srcOffset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << srcOffset << " , " << (srcOffset + bytes_) << " ]");

    OCCA_CHECK((bytes_ + destOffset) <= dest->size,
               "Destination has size [" << dest->size << "],"
               << "trying to access [ " << destOffset << " , " << (destOffset + bytes_) << " ]");

    void *destPtr      = ((char*) (dest->isTexture ? dest->textureInfo.arg : dest->handle)) + destOffset;
    const void *srcPtr = ((char*) (isTexture       ? textureInfo.arg       : handle))       + srcOffset;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::asyncCopyFrom(const void *src,
                                       const uintptr_t bytes,
                                       const uintptr_t offset){
    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << offset << " , " << (offset + bytes_) << " ]");

    void *destPtr      = ((char*) (isTexture ? textureInfo.arg : handle)) + offset;
    const void *srcPtr = src;


    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::asyncCopyFrom(const memory_v *src,
                                       const uintptr_t bytes,
                                       const uintptr_t destOffset,
                                       const uintptr_t srcOffset){
    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + destOffset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << destOffset << " , " << (destOffset + bytes_) << " ]");

    OCCA_CHECK((bytes_ + srcOffset) <= src->size,
               "Source has size [" << src->size << "],"
               << "trying to access [ " << srcOffset << " , " << (srcOffset + bytes_) << " ]");

    void *destPtr      = ((char*) (isTexture      ? textureInfo.arg      : handle))         + destOffset;
    const void *srcPtr = ((char*) (src->isTexture ? src->textureInfo.arg : src->handle)) + srcOffset;;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::asyncCopyTo(void *dest,
                                     const uintptr_t bytes,
                                     const uintptr_t offset){
    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << offset << " , " << (offset + bytes_) << " ]");

    void *destPtr      = dest;
    const void *srcPtr = ((char*) (isTexture ? textureInfo.arg : handle)) + offset;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::asyncCopyTo(memory_v *dest,
                                     const uintptr_t bytes,
                                     const uintptr_t destOffset,
                                     const uintptr_t srcOffset){
    const uintptr_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + srcOffset) <= size,
               "Memory has size [" << size << "],"
               << "trying to access [ " << srcOffset << " , " << (srcOffset + bytes_) << " ]");

    OCCA_CHECK((bytes_ + destOffset) <= dest->size,
               "Destination has size [" << dest->size << "],"
               << "trying to access [ " << destOffset << " , " << (destOffset + bytes_) << " ]");

    void *destPtr      = ((char*) (dest->isTexture ? dest->textureInfo.arg : dest->handle)) + destOffset;
    const void *srcPtr = ((char*) (isTexture       ? textureInfo.arg       : handle))       + srcOffset;

    ::memcpy(destPtr, srcPtr, bytes_);
  }

  template <>
  void memory_t<Serial>::mappedFree(){
    cpu::free(handle);
    handle    = NULL;
    mappedPtr = NULL;

    size = 0;
  }

  template <>
  void memory_t<Serial>::free(){
    if(isTexture){
      cpu::free(textureInfo.arg);
      textureInfo.arg = NULL;
    }
    else{
      cpu::free(handle);
      handle = NULL;
    }

    size = 0;
  }
  //==================================


  //---[ Device ]---------------------
  template <>
  device_t<Serial>::device_t(){
    data = NULL;

    uvaEnabled_ = false;

    bytesAllocated = 0;

    getEnvironmentVariables();

    cpu::addSharedBinaryFlagsTo(compiler, compilerFlags);
  }

  template <>
  device_t<Serial>::device_t(const device_t<Serial> &d){
    *this = d;
  }

  template <>
  device_t<Serial>& device_t<Serial>::operator = (const device_t<Serial> &d){
    modelID_ = d.modelID_;
    id_      = d.id_;

    data = d.data;

    uvaEnabled_    = d.uvaEnabled_;
    uvaMap         = d.uvaMap;
    uvaDirtyMemory = d.uvaDirtyMemory;

    compiler      = d.compiler;
    compilerFlags = d.compilerFlags;

    bytesAllocated = d.bytesAllocated;

    return *this;
  }

  template <>
  void device_t<Serial>::setup(argInfoMap &aim){
    data = new SerialDeviceData_t;

    OCCA_EXTRACT_DATA(Serial, Device);

    data_.vendor = cpu::compilerVendor(compiler);

    cpu::addSharedBinaryFlagsTo(data_.vendor, compilerFlags);
  }

  template <>
  void device_t<Serial>::addOccaHeadersToInfo(kernelInfo &info_){
    info_.addOCCAKeywords(occaSerialDefines);
  }

  template <>
  std::string device_t<Serial>::getInfoSalt(const kernelInfo &info_){
    std::stringstream salt;

    salt << "Serial"
         << info_.salt()
         << parser::version
         << compilerEnvScript
         << compiler
         << compilerFlags;

    return salt.str();
  }

  template <>
  deviceIdentifier device_t<Serial>::getIdentifier() const {
    deviceIdentifier dID;

    dID.mode_ = Serial;

#if (OCCA_OS & (LINUX_OS | OSX_OS))
    const bool debugEnabled = (compilerFlags.find("-g") != std::string::npos);
#else
    const bool debugEnabled = (compilerFlags.find("/Od") != std::string::npos);
#endif

    dID.flagMap["compiler"]     = compiler;
    dID.flagMap["debugEnabled"] = (debugEnabled ? "true" : "false");

    for(int i = 0; i <= 3; ++i){
      std::string flag = "-O";
      flag += '0' + i;

      if(compilerFlags.find(flag) != std::string::npos){
        dID.flagMap["optimization"] = '0' + i;
        break;
      }

      if(i == 3)
        dID.flagMap["optimization"] = "None";
    }

    return dID;
  }

  template <>
  void device_t<Serial>::getEnvironmentVariables(){
    char *c_compiler = getenv("OCCA_CXX");

    if(c_compiler != NULL){
      compiler = std::string(c_compiler);
    }
    else{
      c_compiler = getenv("CXX");

      if(c_compiler != NULL){
        compiler = std::string(c_compiler);
      }
      else{
#if (OCCA_OS & (LINUX_OS | OSX_OS))
        compiler = "g++";
#else
        compiler = "cl.exe";
#endif
      }
    }

    char *c_compilerFlags = getenv("OCCA_CXXFLAGS");

#if (OCCA_OS & (LINUX_OS | OSX_OS))
    if(c_compilerFlags != NULL)
      compilerFlags = std::string(c_compilerFlags);
    else{
#  if OCCA_DEBUG_ENABLED
      compilerFlags = "-g";
#  else
      compilerFlags = "";
#  endif
    }
#else
#  if OCCA_DEBUG_ENABLED
    compilerFlags = " /Od";
#  else
    compilerFlags = " /O2";
#  endif

    std::string byteness;

    if(sizeof(void*) == 4)
      byteness = "x86 ";
    else if(sizeof(void*) == 8)
      byteness = "amd64";
    else
      OCCA_CHECK(false, "sizeof(void*) is not equal to 4 or 8");

    // NBN: adjusted path
#  if      (1800 == _MSC_VER)
    char *visualStudioTools = getenv("VS120COMNTOOLS");   // MSVC++ 12.0 - Visual Studio 2013
#  elif    (1700 == _MSC_VER)
    char *visualStudioTools = getenv("VS110COMNTOOLS");   // MSVC++ 11.0 - Visual Studio 2012
#  else // (1600 == _MSC_VER)
    char *visualStudioTools = getenv("VS100COMNTOOLS");   // MSVC++ 10.0 - Visual Studio 2010
#  endif

    if(visualStudioTools != NULL){
      setCompilerEnvScript("\"" + std::string(visualStudioTools) + "..\\..\\VC\\vcvarsall.bat\" " + byteness);
    }
    else{
      std::cout << "WARNING: Visual Studio environment variable not found -> compiler environment (vcvarsall.bat) maybe not correctly setup." << std::endl;
    }
#endif
  }

  template <>
  void device_t<Serial>::appendAvailableDevices(std::vector<device> &dList){
    device d;
    d.setup("Serial");

    dList.push_back(d);
  }

  template <>
  void device_t<Serial>::setCompiler(const std::string &compiler_){
    compiler = compiler_;

    OCCA_EXTRACT_DATA(Serial, Device);

    data_.vendor = cpu::compilerVendor(compiler);

    cpu::addSharedBinaryFlagsTo(data_.vendor, compilerFlags);
  }

  template <>
  void device_t<Serial>::setCompilerEnvScript(const std::string &compilerEnvScript_){
    compilerEnvScript = compilerEnvScript_;
  }

  template <>
  void device_t<Serial>::setCompilerFlags(const std::string &compilerFlags_){
    OCCA_EXTRACT_DATA(Serial, Device);

    compilerFlags = compilerFlags_;

    cpu::addSharedBinaryFlagsTo(data_.vendor, compilerFlags);
  }

  template <>
  void device_t<Serial>::flush(){}

  template <>
  void device_t<Serial>::finish(){}

  template <>
  bool device_t<Serial>::fakesUva(){
    return false;
  }

  template <>
  void device_t<Serial>::waitFor(streamTag tag){}

  template <>
  stream device_t<Serial>::createStream(){
    return NULL;
  }

  template <>
  void device_t<Serial>::freeStream(stream s){}

  template <>
  stream device_t<Serial>::wrapStream(void *handle_){
    return NULL;
  }

  template <>
  streamTag device_t<Serial>::tagStream(){
    streamTag ret;

    ret.tagTime = currentTime();

    return ret;
  }

  template <>
  double device_t<Serial>::timeBetween(const streamTag &startTag, const streamTag &endTag){
    return (endTag.tagTime - startTag.tagTime);
  }

  template <>
  kernel_v* device_t<Serial>::buildKernelFromSource(const std::string &filename,
                                                    const std::string &functionName,
                                                    const kernelInfo &info_){
    kernel_v *k = new kernel_t<Serial>;
    k->dHandle = this;

    k->buildFromSource(filename, functionName, info_);

    return k;
  }

  template <>
  kernel_v* device_t<Serial>::buildKernelFromBinary(const std::string &filename,
                                                    const std::string &functionName){
    kernel_v *k = new kernel_t<Serial>;
    k->dHandle = this;
    k->buildFromBinary(filename, functionName);
    return k;
  }

  template <>
  void device_t<Serial>::cacheKernelInLibrary(const std::string &filename,
                                              const std::string &functionName,
                                              const kernelInfo &info_){
    //---[ Creating shared library ]----
    kernel tmpK = occa::device(this).buildKernelFromSource(filename, functionName, info_);
    tmpK.free();

    kernelInfo info = info_;

    addOccaHeadersToInfo(info);

    std::string cachedBinary = getCachedName(filename, getInfoSalt(info));

#if (OCCA_OS & WINDOWS_OS)
    // Windows requires .dll extension
    cachedBinary = cachedBinary + ".dll";
#endif
    //==================================

    library::infoID_t infoID;

    infoID.modelID    = modelID_;
    infoID.kernelName = functionName;

    library::infoHeader_t &header = library::headerMap[infoID];

    header.fileID = -1;
    header.mode   = Serial;

    const std::string flatDevID = getIdentifier().flattenFlagMap();

    header.flagsOffset = library::addToScratchPad(flatDevID);
    header.flagsBytes  = flatDevID.size();

    header.contentOffset = library::addToScratchPad(cachedBinary);
    header.contentBytes  = cachedBinary.size();

    header.kernelNameOffset = library::addToScratchPad(functionName);
    header.kernelNameBytes  = functionName.size();
  }

  template <>
  kernel_v* device_t<Serial>::loadKernelFromLibrary(const char *cache,
                                                    const std::string &functionName){
    kernel_v *k = new kernel_t<Serial>;
    k->dHandle = this;
    k->loadFromLibrary(cache, functionName);
    return k;
  }

  template <>
  memory_v* device_t<Serial>::wrapMemory(void *handle_,
                                         const uintptr_t bytes){
    memory_v *mem = new memory_t<Serial>;

    mem->dHandle = this;
    mem->size    = bytes;
    mem->handle  = handle_;

    mem->isAWrapper = true;

    return mem;
  }

  template <>
  memory_v* device_t<Serial>::wrapTexture(void *handle_,
                                          const int dim, const occa::dim &dims,
                                          occa::formatType type, const int permissions){
    memory_v *mem = new memory_t<Serial>;

    mem->dHandle = this;
    mem->size    = ((dim == 1) ? dims.x : (dims.x * dims.y)) * type.bytes();

    mem->isTexture = true;
    mem->textureInfo.dim  = dim;

    mem->textureInfo.w = dims.x;
    mem->textureInfo.h = dims.y;
    mem->textureInfo.d = dims.z;

    mem->textureInfo.arg = handle_;

    mem->handle = &(mem->textureInfo);

    mem->isAWrapper = true;

    return mem;
  }

  template <>
  memory_v* device_t<Serial>::malloc(const uintptr_t bytes,
                                     void *src){
    memory_v *mem = new memory_t<Serial>;

    mem->dHandle = this;
    mem->size    = bytes;

    mem->handle = cpu::malloc(bytes);

    if(src != NULL)
      ::memcpy(mem->handle, src, bytes);

    return mem;
  }

  template <>
  memory_v* device_t<Serial>::textureAlloc(const int dim, const occa::dim &dims,
                                           void *src,
                                           occa::formatType type, const int permissions){
    memory_v *mem = new memory_t<Serial>;

    mem->dHandle = this;
    mem->size    = ((dim == 1) ? dims.x : (dims.x * dims.y)) * type.bytes();

    mem->isTexture = true;
    mem->textureInfo.dim  = dim;

    mem->textureInfo.w = dims.x;
    mem->textureInfo.h = dims.y;
    mem->textureInfo.d = dims.z;

    mem->handle = cpu::malloc(mem->size);

    ::memcpy(mem->textureInfo.arg, src, mem->size);

    mem->handle = &(mem->textureInfo);

    return mem;
  }

  template <>
  memory_v* device_t<Serial>::mappedAlloc(const uintptr_t bytes,
                                          void *src){
    memory_v *mem = malloc(bytes, src);

    mem->mappedPtr = mem->handle;

    return mem;
  }

  template <>
  void device_t<Serial>::free(){}

  template <>
  int device_t<Serial>::simdWidth(){
    simdWidth_ = OCCA_SIMD_WIDTH;
    return OCCA_SIMD_WIDTH;
  }
  //==================================
};
