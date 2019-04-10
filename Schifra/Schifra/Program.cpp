
#include <cstddef>
#include <iostream>
#include <string>

#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"

int main()
{
    /* Finite Field Parameters */
   const std::size_t field_descriptor                =   2;
   const std::size_t generator_polynomial_index      =   2;
   const std::size_t generator_polynomial_root_count =   1;

   /* Reed Solomon Code Parameters */
   const std::size_t code_length = 11;
   const std::size_t fec_length  =  2;
   const std::size_t data_length = code_length - fec_length;

   /* Instantiate Finite Field and Generator Polynomials */
   const schifra::galois::field field(field_descriptor,
                                      schifra::galois::primitive_polynomial_size41,
                                      schifra::galois::primitive_polynomial41);

   schifra::galois::field_polynomial generator_polynomial(field);	

   if (
        !schifra::make_sequential_root_generator_polynomial(
                     field,
                     generator_polynomial_index,
                     generator_polynomial_root_count,
                     generator_polynomial)
      )
   {
      std::cout << "Error - Failed to create sequential root generator!" << std::endl;
      return 1;
   }

   /* Instantiate Encoder and Decoder (Codec) */
   typedef schifra::reed_solomon::encoder<code_length,fec_length,data_length> encoder_t;
   typedef schifra::reed_solomon::decoder<code_length,fec_length,data_length> decoder_t;

   const encoder_t encoder(field,generator_polynomial);
   const decoder_t decoder(field,generator_polynomial_index);

   char buffer[11];
   
   for (char i = 0; i < 9; i++)
   {
	   buffer[i] = i+1;
   }

   buffer[9] = 0;
   buffer[10] = 0;

   std::string message(buffer, 11);;

   /* Pad message with nulls up until the code-word length */
   message.resize(code_length,0x00);

   /* Instantiate RS Block For Codec */
   schifra::reed_solomon::block<code_length,fec_length> block;

   /* Transform message into Reed-Solomon encoded codeword */
   if (!encoder.encode(message,block))
   {
      std::cout << "Error - Critical decoding failure! "
                << "Msg: " << block.error_as_string()  << std::endl;
      return 1;
   }

   std::cout << "Encoded Message: " << message  << std::endl;

   return 0;
}

