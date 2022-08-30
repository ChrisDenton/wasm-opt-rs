#ifndef wasmopt_shims_h
#define wasmopt_shims_h

#include "pass.h"
#include "wasm-io.h"
#include "support/colors.h"
#include "wasm-validator.h"

#include <stdexcept> // runtime_error
#include <memory> // unique_ptr

namespace wasm_shims {
  std::runtime_error parse_exception_to_runtime_error(const wasm::ParseException& e) {
    std::ostringstream buf;
    e.dump(buf);
    
    return std::runtime_error(buf.str());
  }
}

namespace wasm_shims {
  bool validateWasm(wasm::Module& wasm) {
    wasm::WasmValidator v;

    return v.validate(wasm);
  }
  
}

namespace wasm_shims {
  typedef wasm::Module Module;

  std::unique_ptr<Module> newModule() {
    return std::make_unique<Module>();
  }
}

namespace wasm_shims {
  struct ModuleReader {
    wasm::ModuleReader inner;

    void setDebugInfo(bool debug) {
      inner.setDebugInfo(debug);
    }

    void setDwarf(bool dwarf) {
      inner.setDWARF(dwarf);
    }

    // Wrapper to handle by-val string.
    void readText(const std::string& filename, Module& wasm) {
      try {
        inner.readText(std::string(filename), wasm);
      } catch (const wasm::ParseException &e) {
        throw parse_exception_to_runtime_error(e);
      }
    }

    void readBinary(const std::string& filename,
                                 Module& wasm,
                                 const std::string& sourceMapFilename) {
      try {
        inner.readBinary(std::string(filename),
                          wasm,
                          std::string(sourceMapFilename));
      } catch (const wasm::ParseException &e) {
        throw parse_exception_to_runtime_error(e);
      }
    }

    void read(const std::string& filename,
              Module& wasm,
              const std::string& sourceMapFilename) {
      try {
        inner.readBinary(std::string(filename),
                          wasm,
                          std::string(sourceMapFilename));
      } catch (const wasm::ParseException &e) {
        throw parse_exception_to_runtime_error(e);
      }
    }
  };

  std::unique_ptr<ModuleReader> newModuleReader() {
    return std::make_unique<ModuleReader>();
  }
}

namespace wasm_shims {
  struct ModuleWriter {
    wasm::ModuleWriter inner;

    void setDebugInfo(bool debug) {
      inner.setDebugInfo(debug);
    }

    void setSourceMapFilename(const std::string& source_map_filename) {
      inner.setSourceMapFilename(source_map_filename);
    }

    void setSourceMapUrl(const std::string& source_map_url) {
      inner.setSourceMapUrl(source_map_url);
    }
  
    void writeText(Module& wasm,
                   const std::string& filename) {
      inner.writeText(wasm, std::string(filename));
    }

    void writeBinary(Module& wasm,
                     const std::string& filename) {
      inner.writeBinary(wasm, std::string(filename));
    }
  };
    
  std::unique_ptr<ModuleWriter> newModuleWriter() {
    return std::make_unique<ModuleWriter>();
  }
}

namespace wasm_shims {
  std::unique_ptr<std::vector<std::string>> getRegisteredNames() {
    auto r = wasm::PassRegistry::get();
    return std::make_unique<std::vector<std::string>>(r->getRegisteredNames());
  }

  std::unique_ptr<std::string> getPassDescription(const std::string& name) {
    auto r = wasm::PassRegistry::get();
    return std::make_unique<std::string>(r->getPassDescription(std::string(name)));
  }

  bool isPassHidden(const std::string& name) {
    auto r = wasm::PassRegistry::get();
    return r->isPassHidden(std::string(name));
  }
}

namespace wasm_shims {
  struct InliningOptions {
    wasm::InliningOptions inner;

    void setAlwaysInlineMaxSize(uint32_t size) {
      inner.alwaysInlineMaxSize = size;
    }

    void setOneCallerInlineMaxSize(uint32_t size) {
      inner.oneCallerInlineMaxSize = size;
    }

    void setFlexibleInlineMaxSize(uint32_t size) {
      inner.flexibleInlineMaxSize = size;
    }

    void setAllowFunctionsWithLoops(bool allow) {
      inner.allowFunctionsWithLoops = allow;
    }

    void setPartialInliningIfs(uint32_t number) {
      inner.partialInliningIfs = number;
    }
  };
    
  std::unique_ptr<InliningOptions> newInliningOptions() {
    return std::make_unique<InliningOptions>();
  }
}

namespace wasm_shims {
  struct PassOptions {
    wasm::PassOptions inner;

    void setValidate(bool validate) {
      inner.validate = validate;
    }

    void setValidateGlobally(bool validate) {
      inner.validateGlobally = validate;
    }

    void setOptimizeLevel(int32_t level) {
      inner.optimizeLevel = level;
    }

    void setShrinkLevel(int32_t level) {
      inner.shrinkLevel = level;
    }

    void setInliningOptions(std::unique_ptr<wasm_shims::InliningOptions> inlining) {
      inner.inlining = inlining->inner;
    }
    
    void setTrapsNeverHappen(bool ignoreTraps) {
      inner.trapsNeverHappen = ignoreTraps;
    }

    void setLowMemoryUnused(bool memoryUnused) {
      inner.lowMemoryUnused = memoryUnused;
    }

    void setFastMath(bool fastMath) {
      inner.fastMath = fastMath;
    }

    void setZeroFilledMemory(bool zeroFilledMemory) {
      inner.zeroFilledMemory = zeroFilledMemory;
    }

    void setDebugInfo(bool debugInfo) {
      inner.debugInfo = debugInfo;
    }
  };

  std::unique_ptr<PassOptions> newPassOptions() {
    return std::make_unique<PassOptions>();
  }
}

namespace wasm_shims {
  struct PassRunner {
    wasm::PassRunner inner;

    PassRunner(Module* wasm) : inner(wasm::PassRunner(wasm)) {}
    PassRunner(Module* wasm, PassOptions options) : inner(wasm::PassRunner(wasm, options.inner)) {}

    void add(const std::string& passName) {
      inner.add(std::string(passName));
    }

    void addDefaultOptimizationPasses() {
      inner.addDefaultOptimizationPasses();
    }

    void run() {
      inner.run();
    }
  };

  std::unique_ptr<PassRunner> newPassRunner(Module& wasm) {
    return std::make_unique<PassRunner>(&wasm);
  }

  std::unique_ptr<PassRunner> newPassRunnerWithOptions(Module& wasm, std::unique_ptr<wasm_shims::PassOptions> options) {
    return std::make_unique<PassRunner>(&wasm, *options);
  }

  bool passRemovesDebugInfo(const std::string& name) {
    return wasm::PassRunner::passRemovesDebugInfo(std::string(name));
  }

  bool checkInliningOptionsDefaults(std::unique_ptr<InliningOptions> inlining) {
    auto inliningOptionsDefaults = wasm_shims::newInliningOptions();

    // The size assertion will fail when `InliningOptions` fields change,
    // which indicates the current test need to be updated.
    assert(sizeof(inliningOptionsDefaults->inner) == 20);
    
    bool isEqual = (inlining->inner.alwaysInlineMaxSize == inliningOptionsDefaults->inner.alwaysInlineMaxSize)
      && (inlining->inner.oneCallerInlineMaxSize == inliningOptionsDefaults->inner.oneCallerInlineMaxSize)
      && (inlining->inner.flexibleInlineMaxSize == inliningOptionsDefaults->inner.flexibleInlineMaxSize)
      && (inlining->inner.allowFunctionsWithLoops == inliningOptionsDefaults->inner.allowFunctionsWithLoops)
      && (inlining->inner.partialInliningIfs == inliningOptionsDefaults->inner.partialInliningIfs);
    
    return isEqual;
  }

  bool checkPassOptionsDefaults(std::unique_ptr<PassOptions> passOptions) {
    auto passOptionsDefaults = wasm::PassOptions::getWithDefaultOptimizationOptions();

    // The size assertion will fail when `PassOptions` or `InliningOptions` fields change.
    // We need to update the test when the size assertion failed.
    //    assert(sizeof(passOptionsDefaults) == 64); // Unix
    //    assert(sizeof(passOptionsDefaults) == 56); // Windows
    //    assert(sizeof(passOptionsDefaults) == 88); // Ubuntu
    std::cout << " -------- size of optimizationsOptions " << sizeof(passOptionsDefaults);

    bool isEqual = (passOptions->inner.debug == passOptionsDefaults.debug)
      && (passOptions->inner.validate == passOptionsDefaults.validate)
      && (passOptions->inner.validateGlobally == passOptionsDefaults.validateGlobally)
      && (passOptions->inner.optimizeLevel == passOptionsDefaults.optimizeLevel) 
      && (passOptions->inner.shrinkLevel == passOptionsDefaults.shrinkLevel)
      && (passOptions->inner.trapsNeverHappen == passOptionsDefaults.trapsNeverHappen)
      && (passOptions->inner.lowMemoryUnused == passOptionsDefaults.lowMemoryUnused)
      && (passOptions->inner.fastMath == passOptionsDefaults.fastMath)
      && (passOptions->inner.zeroFilledMemory == passOptionsDefaults.zeroFilledMemory)
      && (passOptions->inner.debugInfo == passOptionsDefaults.debugInfo)
      // inlining fields comparison
      && (passOptions->inner.inlining.alwaysInlineMaxSize == passOptionsDefaults.inlining.alwaysInlineMaxSize)
      && (passOptions->inner.inlining.oneCallerInlineMaxSize == passOptionsDefaults.inlining.oneCallerInlineMaxSize)
      && (passOptions->inner.inlining.flexibleInlineMaxSize == passOptionsDefaults.inlining.flexibleInlineMaxSize)
      && (passOptions->inner.inlining.allowFunctionsWithLoops == passOptionsDefaults.inlining.allowFunctionsWithLoops)
      && (passOptions->inner.inlining.partialInliningIfs == passOptionsDefaults.inlining.partialInliningIfs);
   
    return isEqual;
  }
}

#endif // wasmopt_shims_h

