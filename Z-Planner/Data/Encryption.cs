using System;
using System.Globalization;
using System.IO;
using System.Security.Cryptography;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace Crypto
{
    class Encryption
    {
        // Hold information related to the key and vector used during encryption/decryption
        private byte[] Key;
        private byte[] Vector;
        private Rijndael RijndaelAlg;


        // Constructor
        //
        public Encryption()
        {
            RijndaelAlg = Rijndael.Create();

            // Save the Key and Initialization Vector locally
            Key = RijndaelAlg.Key;
            Vector = RijndaelAlg.IV;
        }

        public Encryption(string sKeys)
        {
            RijndaelAlg = Rijndael.Create();
            SetKeys(sKeys);
        }


        // Property to get the encryption key
        public byte[] RijndaelKey
        {
            get { return Key; }
            set { Key = value; }
        }


        // Property to get the initialization vector
        public byte[] RijndaelVector
        {
            get { return Vector; }
            set { Vector = value; }
        }


        // Decrypt file 
        public string Decrypt(string fileName)
        {
            string result = "";

            if (File.Exists(fileName))
            {

                FileStream fStream = new FileStream(fileName, FileMode.Open);
                CryptoStream decryptStream = new CryptoStream(fStream, RijndaelAlg.CreateDecryptor(Key, Vector), CryptoStreamMode.Read);
                StreamReader reader = new StreamReader(decryptStream);


                try
                {
                    result = reader.ReadToEnd();
                }
                catch (Exception decryptEx)
                {
                    throw new Exception("Error decrypting source file. Error: " + decryptEx.Message);
                }
                finally
                {
                    reader.Close();
                    decryptStream.Close();
                    fStream.Close();
                }
            }
            else
            {
                throw new Exception("The source file could not be located. File name: " + fileName);
            }

            return result;
        }


        // Encrypt data and save it into the specified file. 
        public string Encrypt(string data, string fileName)
        {
            string result = "";
            try
            {
                FileStream fStream = new FileStream(fileName, FileMode.OpenOrCreate);
                CryptoStream cryptStream = new CryptoStream(fStream, RijndaelAlg.CreateEncryptor(Key, Vector), CryptoStreamMode.Write);
                StreamWriter writer = new StreamWriter(cryptStream);

                try
                {
                    writer.Write(data);
                    result = GetKeys();
                }
                catch (Exception streamException)
                {
                    throw new Exception("Error saving the data to file. Error: " + streamException.Message);
                }
                finally
                {
                    writer.Close();
                    cryptStream.Close();
                    fStream.Close();
                }
            }
            catch (Exception ex)
            {
                throw new Exception("There was a problem saving the data to file. Error: " + ex.Message);
            }

            return result;
        }

        private void SetKeys(string sKeys)
        {
            byte[] check = sKeys.Split('-')
                    .Select(x => byte.Parse(x, NumberStyles.HexNumber))
                    .ToArray();


            byte[] decL = new byte[4];
            Array.Copy(check, 0, decL, 0, 4);
            int idecK = BitConverter.ToInt32(decL, 0);

            byte[] decK = new byte[idecK];
            byte[] decV = new byte[check.Length - idecK - 4];
            Array.Copy(check, 4, decK, 0, idecK);
            Array.Copy(check, idecK + 4, decV, 0, check.Length - idecK - 4);

            Key = decK;
            Vector = decV;
        }

        public string GetKeys()
        {
            //store keys
            byte[] k = RijndaelKey;
            byte[] v = RijndaelVector;

            byte[] kLen = BitConverter.GetBytes(k.Length);
            byte[] keys = new byte[kLen.Length + k.Length + v.Length];

            Array.Copy(kLen, 0, keys, 0, kLen.Length);
            Array.Copy(k, 0, keys, kLen.Length, k.Length);
            Array.Copy(v, 0, keys, kLen.Length + k.Length, v.Length);

            string sKeys = BitConverter.ToString(keys);
            return sKeys;
        }
    }
}