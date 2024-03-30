
![squidbasic](https://github.com/hashsquid/hashsquid/assets/162920851/fb1bb752-90bb-4524-9895-bdb50a94dd5d)

[Intro](#introduction)

[Technical Details](#technical-details)

[Contract Write Functions](#contract-write-functions)

[Contract Read Functions](#contract-read-functions)

[Finding Hashes (Python)](#finding-hashes-python)

[Finding Hashes (C)](#finding-hashes-c)
    

## Introduction
  **Prize pool NFT** collection on Base with a guaranteed minimum one-time prize of **1.0000384 eth**.

  There are a total of 256 hashSquid to be minted
  
  This is a unique twist on the gamification of NFTs. The objective in this game get a low `hashsum` on your hashSquid NFT(s). After the final hashSquid is minted, the hashSquid with the lowest `hashsum` wins the prize, which has a guaranteed minimum of 1.0000384 eth. 

  Each hashSquid NFT has two attributes: a `baseHash` (bytes32) and a `hashsum` (uint16). To update a hashSquid NFT's `hashsum`, an `input` (bytes32) must be found where `hashSum(keccak256(baseHash, input))` is below the NFT's current `hashsum`.  (See helper scripts to find good hashes in [Python](#finding-hashes-python) and [C](#finding-hashes-c))

  After the 256th NFT is minted, the owner of the hashSquid NFT with the lowest `hashsum` gets 95% of the contract balance, and the contract creator gets 5%.  

  The contract balance comes from minting and `hashsum` updating fees: 

Minting fee: **0.004112 eth**

`hashsum` update fee: **0.002056 eth**

### Minting:

    -- 256 total NFTs
    -- Minting fee is 0.004112 eth
    -- Fees accumulate from minting and hashsum updates
    -- When the final NFT is minted, the owner of the NFT with the lowest guardSum is paid 95% of the contract balance (accrued from fees)
    -- NFTs are assigned a baseHash and hashsum upon minting. The initial hashsum will be around 4080 (see Technical Details) and needs to be updated to compete effectively for the prize pool
    
### hashsum updating:

    -- Before the final NFT is minted, the hashsum updating (function: updateHashsum) fee is 0.002056 eth. After that, there is no fee
    -- An NFT's hashsum can be updated (owner only) if an input hash is found where hashSum(keccak256(baseHash, input)) < current hashsum
    -- An NFT's baseHash changes upon successfully updating the hashsum

Base CA: 0xfEC66319b6ebC37f33E2aEcb00C40B21b57dE5B8

Basic Contract Interaction: http://hashsquid.com/

## Technical Details
  Consider the keccak256 hash of `1234` (string): 
  `0x387a8233c96e1fc0ad5e284353276177af2186e7afa85296f106336e376669f7`. 
  
  This hash can be expressed as byte array (with possible values 0-255): 
  `[56,122,130,51,201,110,31,192,173,94,40,67,83,39,97,119,175,33,134,231,175,168,82,150,241,6,51,110,55,102,105,247] `
  
  the `hashSum` of this byte array is just the sum of the decimals at each position, so `3670` in this case. The histogram below shows the `hashsum` distribution for 100,000 `keccak256` hashes:
  
  ![hash_dist](https://github.com/hashsquid/hashsquid/assets/162920851/146f6b2f-6001-4bb8-ab3a-9d408ad23fba)

   The average `keccak256 hashsum` after 100,000 iterations is 4080 = (8160/2). The distribution is normally distributed around the average; finding `hashsums` further away from 4080 in either +/- direction becomes increasingly difficult. One out of 2^256 `inputs` is expected to give a `keccak256` hash with all zeros, i.e. with a `hashsum` of zero. Since the maximum possible value in the byte array is 255, and there are 32 positions in the array, the maximum possible `keccak256 hashsum` is 8160. 

  Each hashSquid NFT maps to a `baseHash`, and to update an NFT's `hashsum`, an `input` (bytes32) must be found which hashes together with the NFT's `baseHash` to produce a `hashsum` less than the NFT's current `guardSum`. When an NFT's `hashsum` is updated, its `baseHash` changes.
  
  Let's say a hashSquid's `baseHash` is `0x387a8233c96e1fc0ad5e284353276177af2186e7afa85296f106336e376669f7` and its current `hashsum` is `2150`. First, let's test `input` (bytes32) of `0x47a4ac6742dabb8c531c6e2b6fba383e56619dec9bdb4937dd50a707bf24bd02`.
  
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
  `2143 < 2150` is `true`, so this `input` works. Now the `hashsum` can be updated. We just call `updateHashsum(input (bytes32), tokenID)` on the contract.

  ## Contract Write Functions:
  For now, there are two ways to call these functions:
  - [basescan](https://basescan.org/address/0xfec66319b6ebc37f33e2aecb00c40b21b57de5b8#writeContract)
  - http://hashsquid.com/
  
  **Mint** 
  
    No input accepted. Max 256 mintings. 
    Minted NFT's baseHash set to `keccak256(block.timestamp, msg.sender)
    Minted NFT's hashsum set to `keccak256(msg.sender, tokenId)

  **updateHashsum** `(bytes32 data, uint256 tokenId)`
  
    Only the owner of the NFT with tokenId can call this function
    Checks if hashSum(keccak256(baseHash, data)) < current hashsum
    If it is, update the NFT's hashsum
  
## Contract Read Functions:

  **getBaseHash** `()`

    Given an NFT tokenId, return the current baseHash

  **getHashsum**  `()`

    Given an NFT tokenId, return the current `hashsum`
  
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
  
