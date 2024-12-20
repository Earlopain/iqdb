#include <format>
#include <vector>

#include <iqdb/haar_signature.h>
#include <iqdb/haar.h>
#include <iqdb/imgdb.h>

namespace iqdb {

HaarSignature::HaarSignature(lumin_t avglf_, signature_t sig_) {
  std::copy(avglf_, avglf_+ 3, avglf);
  std::copy(&sig_[0][0], &sig_[0][0] + 3*40, &sig[0][0]);

  std::sort(&sig[0][0], &sig[0][NUM_COEFS]);
  std::sort(&sig[1][0], &sig[1][NUM_COEFS]);
  std::sort(&sig[2][0], &sig[2][NUM_COEFS]);
}

HaarSignature HaarSignature::from_channels(std::vector<unsigned char> rchan, std::vector<unsigned char> gchan, std::vector<unsigned char> bchan) {
  HaarSignature signature;

  std::vector<Unit> cdata1(NUM_PIXELS * NUM_PIXELS);
  std::vector<Unit> cdata2(NUM_PIXELS * NUM_PIXELS);
  std::vector<Unit> cdata3(NUM_PIXELS * NUM_PIXELS);
  transformChar(rchan.data(), gchan.data(), bchan.data(), cdata1.data(), cdata2.data(), cdata3.data());
  calcHaar(cdata1.data(), cdata2.data(), cdata3.data(), signature.sig[0], signature.sig[1], signature.sig[2], signature.avglf);

  std::sort(&signature.sig[0][0], &signature.sig[0][NUM_COEFS]);
  std::sort(&signature.sig[1][0], &signature.sig[1][NUM_COEFS]);
  std::sort(&signature.sig[2][0], &signature.sig[2][NUM_COEFS]);

  return signature;
}

HaarSignature HaarSignature::from_hash(const std::string hash) {
  if (hash.size() != 2 * sizeof(HaarSignature)) {
    throw param_error("Invalid hash (hash=" + hash + ")");
  }

  HaarSignature haar;
  const char* p = hash.c_str();

  for (double& avglf : haar.avglf) {
    sscanf(p, "%16lx", reinterpret_cast<uint64_t*>(&avglf));
    p += 2 * sizeof(uint64_t);
  }

  for (int c = 0; c < 3; c++) {
    for (int16_t& coef : haar.sig[c]) {
      sscanf(p, "%4hx", reinterpret_cast<uint16_t*>(&coef));
      p += 2 * sizeof(int16_t);
    }
  }

  return haar;
}

std::string HaarSignature::to_string() const {
  std::string str = "";
  str.reserve(sizeof(HaarSignature)*2);

  str += std::format("{:016x}", reinterpret_cast<const uint64_t&>(avglf[0]));
  str += std::format("{:016x}", reinterpret_cast<const uint64_t&>(avglf[1]));
  str += std::format("{:016x}", reinterpret_cast<const uint64_t&>(avglf[2]));

  for (size_t c = 0; c < 3; c++) {
    for (size_t i = 0; i < NUM_COEFS; i++) {
      str += std::format("{:04x}", reinterpret_cast<const uint16_t&>(sig[c][i]));
    }
  }

  return str;
}

bool HaarSignature::is_grayscale() const noexcept {
  return std::abs(avglf[1]) + std::abs(avglf[2]) < 6.0 / 1000;
}

int HaarSignature::num_colors() const noexcept {
  return is_grayscale() ? 1 : 3;
}

}
