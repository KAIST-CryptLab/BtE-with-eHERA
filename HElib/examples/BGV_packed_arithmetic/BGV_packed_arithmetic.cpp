/* Copyright (C) 2019-2021 IBM Corp.
 * This program is Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */

// This is a sample program for education purposes only.
// It attempts to show the various basic mathematical
// operations that can be performed on both ciphertexts
// and plaintexts.

#include <iostream>

#include <helib/helib.h>
#include <helib/PAlgebra.h>

int main(int argc, char* argv[])
{
  /*  Example of BGV scheme  */

  // Plaintext prime modulus
  unsigned long p = 2;
  // Cyclotomic polynomial - defines phi(m)
  // unsigned long m = 32109;
  unsigned long m = 30269;
  // Hensel lifting (default = 1)
  unsigned long r = 1;
  // Number of bits of the modulus chain
  unsigned long bits = 500;
  // Number of columns of Key-Switching matrix (default = 2 or 3)
  unsigned long c = 2;

  std::cout << "\n*********************************************************";
  std::cout << "\n*         Basic Mathematical Operations Example         *";
  std::cout << "\n*         =====================================         *";
  std::cout << "\n*                                                       *";
  std::cout << "\n* This is a sample program for education purposes only. *";
  std::cout << "\n* It attempts to show the various basic mathematical    *";
  std::cout << "\n* operations that can be performed on both ciphertexts  *";
  std::cout << "\n* and plaintexts.                                       *";
  std::cout << "\n*                                                       *";
  std::cout << "\n*********************************************************";
  std::cout << std::endl;

  std::cout << "Initialising context object..." << std::endl;
  // Initialize context
  // This object will hold information about the algebra created from the
  // previously set parameters
  helib::Context context = helib::ContextBuilder<helib::BGV>()
                               .m(m)
                               .p(p)
                               .r(r)
                               .bits(bits)
                               .c(c)
                               .build();

  // Print the context
  context.printout();
  std::cout << std::endl;

  // Print the security level
  std::cout << "Security: " << context.securityLevel() << std::endl;

  // Secret key management
  std::cout << "Creating secret key..." << std::endl;
  // Create a secret key associated with the context
  helib::SecKey secret_key(context);
  // Generate the secret key
  secret_key.GenSecKey();
  std::cout << "Generating key-switching matrices..." << std::endl;
  // Compute key-switching matrices that we need
  helib::addSome1DMatrices(secret_key);

  // Public key management
  // Set the secret key (upcast: SecKey is a subclass of PubKey)
  const helib::PubKey& public_key = secret_key;

  // Build the EncryptedArray of the context
  helib::PAlgebraMod pAlgMod = context.getAlMod();
  helib::PAlgebraModDerived<helib::PA_GF2> pAlgModDer(pAlgMod.getZMStar(), 1);
  helib::PA_GF2::RX factor = pAlgModDer.getFactors()[0];
  const helib::EncryptedArrayDerived<helib::PA_GF2> ea2(context, factor, context.getAlMod());
  std::cout << "factor: " << factor << std::endl;
  std::cout << "r: " << pAlgModDer.getR() << std::endl;

  // Get the number of slot (phi(m))
  long nslots = ea2.size();
  std::cout << "Number of slots: " << nslots << std::endl;

  std::vector<NTL::GF2X> data(nslots, NTL::GF2X::zero());
  std::cout << "Mult const:" << std::endl;
  for (long i = 0; i < nslots; i++) {
    SetX(data[i]);
    if (i <= 2)
      std::cout << "  const[" << i << "]: " << data[i] << std::endl;
  }
  std::cout << "..." << std::endl;
  NTL::ZZX ptxt_x;
  ea2.encode(ptxt_x, data);
  std::cout << std::endl;

  std::vector<NTL::GF2X> data2(nslots, NTL::GF2X(689, NTL::GF2::zero()));
  std::cout << "Input ptxt (2^689):" << std::endl;
  for (long i = 0; i < nslots; i++) {
    SetCoeff(data2[i], 689);
    if (i <= 2)
      std::cout << "  ptxt[" << i << "]" << data2[i] << std::endl;
  }
  std::cout << "..." << std::endl;
  NTL::ZZX ptxt_689;
  ea2.encode(ptxt_689, data2);

  helib::Ctxt ctxt_ff(public_key);
  public_key.Encrypt(ctxt_ff, ptxt_689);
  
  NTL::ZZX dec_ff;
  std::vector<NTL::GF2X> res;
  for (int i = 1; i <= 5; i++) {
    ctxt_ff.multByConstant(ptxt_x);
    secret_key.Decrypt(dec_ff, ctxt_ff);

    ea2.decode(res, dec_ff);
    std::cout << "ptxt x 2^" << i << ":" << std::endl;
    for (int j = 0; j <= 2; j++)
      std::cout << "  val[" << j << "]: " << res[j] << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
