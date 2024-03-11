
![squidbasic](https://github.com/hashsquid/hashsquid/assets/162920851/fb1bb752-90bb-4524-9895-bdb50a94dd5d)

[Intro](#introduction)

[Technical Details](#technical-details)

[Contract Write Functions](#contract-write-functions)

[Contract Read Functions](#contract-read-functions)

[Finding Hashes (Python)](#finding-hashes-python)

[Finding Hashes (C)](#finding-hashes-c)
    

## Introduction
  The HashSquid project features 256 generative art hashSquid **NFTs which are able to be _stolen_.** 
  
  This is a unique twist on the gamification of NFTs. The objective in this game is to obtain and maintain ownership of hashSquid NFTs, which can be stolen. 

  Each hashSquid NFT maps to a `baseHash` (bytes32) and a `guardSum` (uint16). To steal an NFT, an `input` (bytes32) must be found where `hashSum(keccak256(baseHash, input))` is below the NFT's current `guardSum`. So, the `guardSum` protects the NFT from being stolen: the lower it is, the more difficult it is to steal. (See helper scripts to find hashes in [Python](#finding-hashes-python) and [C](#finding-hashes-c))

  hashSquid NFTs with low `guardSums` are worth more, since they are harder to steal. 

  Minting a hashSquid only costs gas fees, and there is no `input` (bytes32) requirement.

  FTM CA: 

## Technical Details
  Consider the keccak256 hash of `1234` (string): 
  `0x387a8233c96e1fc0ad5e284353276177af2186e7afa85296f106336e376669f7`. 
  
  This hash can be expressed as an array of HexBytes (with possible values 0-255): 
  `[56,122,130,51,201,110,31,192,173,94,40,67,83,39,97,119,175,33,134,231,175,168,82,150,241,6,51,110,55,102,105,247] `
  
  the `hashSum` of this byte array is just the sum of the decimals at each position, so `3670` in this case. The histogram below shows the `hashSum` distribution for 100,000 random `keccak256` hashes:
  
  ![hash_dist](https://github.com/hashsquid/hashsquid/assets/162920851/146f6b2f-6001-4bb8-ab3a-9d408ad23fba)

  Since the maximum possible value in the byte array is 255, and there are 32 positions in the array, the maximum possible `keccak256 hashSum` is 8160. Half this value is 4080, and this is the average `keccak256 hashSum` after 100,000 iterations. So the distribution is normally distributed around 4080; finding `hashSums` further away from 4080 in either +/- direction becomes increasingly difficult. One out of 2^256 `inputs` is expected to give a `keccak256` hash with all zeros, i.e. with a `hashSum` of zero

  Each hashSquid NFT maps to a `baseHash`, and to steal an NFT, an `input` (bytes32) must be found which hashes together with the NFT's current `baseHash` to produce a `hashSum` less than the NFT's current `guardSum`. 
  
  Let's say an NFT's current `baseHash` is `0x387a8233c96e1fc0ad5e284353276177af2186e7afa85296f106336e376669f7` and the NFT's current `guardSum` is `2150`. First, let's test `input` (bytes32) of `0x47a4ac6742dabb8c531c6e2b6fba383e56619dec9bdb4937dd50a707bf24bd02`.
  
```
keccak256(0x387a8233c96e1fc0ad5e284353276177af2186e7afa85296f106336e376669f7, 0x47a4ac6742dabb8c531c6e2b6fba383e56619dec9bdb4937dd50a707bf24bd02), =
0x341b03593d0724275b8a4be9cb2931c2175a629a320e0c14011981fa3668404a =
[52,27,3,89,61,7,36,39,91,138,75,233,203,41,49,194,23,90,98,154,50,14,12,20,1,25,129,250,54,104,64,74] =
hashSum 2500
  ```
  `2500 < 2150` is `false`, so this `input` doesn't work. Now let's try `input` of `0x464644dbcbdae1877b785211cd97613701b04b286387a3a673f5cff6952f777b`.

  ```
  keccak256(0x387a8233c96e1fc0ad5e284353276177af2186e7afa85296f106336e376669f7, 0x464644dbcbdae1877b785211cd97613701b04b286387a3a673f5cff6952f777b) =
0x276327ee126b2509c37209ca127d2a3e4f03280657541e0d865d4521020d1751 =
[39,99,39,238,18,107,37,9,195,114,9,202,18,125,42,62,79,3,40,6,87,84,30,13,134,93,69,33,2,13,23,81] =
hashSum 2143
  ```
  `2143 < 2150` is `true`, so this `input` works. Now the NFT can be stolen. We just call `reflip(input (bytes32), tokenID)` on the contract, and the NFT will be transferred to `msg.sender`. 

  ## Contract Write Functions:
  For now, the easiest way to call these functions is on the ftmscan blockchain explorer.
  
  **Mint** 
  
    No input accepted. Max 256 mintings. 
    Minted NFT's baseHash set to `keccak256(block.timestamp, msg.sender)
    Minted NFT's guardSum set to `keccak256(msg.sender, tokenId)

  **updateGuardSum** `(bytes32 data, uint256 tokenId)`
  
    Only the owner of the NFT with tokenId can call this function
    Checks the hashSum(keccak256(baseHash, data)) < guardSum
    If it is, reset baseHash to keccak256(block.timestamp, msg.sender, hashSum)
    and update the NFT's guardSum

  **reflip**  `(bytes32 data, uint256 tokenId)`
  
    i.e. pilfer, get it?
    Only non-owners can call this function
    Checks the hashSum(keccak256(baseHash, data)) < guardSum
    If it is, reset baseHash to keccak256(block.timestamp, msg.sender, hashSum)
    and update guardSum
    and transfer ownership to msg.sender
  
## Contract Read Functions:

  **getBaseHash** `()`

    Given an NFT tokenId, return the current baseHash

  **getGuardSum**  `()`

    Given an NFT tokenId, return the current guardSum
  
  
  
  ## Finding hashes (python)
        
        python find_hash.py <guardSum> <baseHash (with 0x)>
        
  ## Finding hashes (C)
    
      git clone https://github.com/brainhub/SHA3IUF
      cd SHA3IUF
      cp ../find_hash.c ./
      gcc -o find_hash find_hash.c sha3.c
      ./find_hash <guardSum> <baseHash (without 0x)>

  
  ## TODO 

      update find_hash.c to work on GPU
  
