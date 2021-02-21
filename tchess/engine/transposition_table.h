/*
 * tansposition_table.h
 *
 *  Created on: 2021. febr. 9.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_ENGINE_TRANSPOSITION_TABLE_H_
#define SRC_ENGINE_TRANSPOSITION_TABLE_H_

#include <utility>
#include <array>

#include "polyglot.h" //<- for the 781 random numbers and uint typedef

namespace tchess
{
	/*
	 * Creates a 64 bit hash from a position.
	 */
	uint64 createZobrishHash(const chessboard& board, const game_information& info);

	/*
	 * Constants for the 3 types of entries in the transposition table. Extra type is
	 * for
	 */
	extern const unsigned short exact, upperBound, lowerBound, uninitialized;

	/*
	 * Represents an entry in the transposition table.
	 */
	struct transposition_entry {

		//Hash of this entry.
		uint64 hashKey;

		//One of the exact, upperBound, lowerBound constants (or the uninitialized).
		unsigned short entryType;

		//The depth in the search where this entry was evaluated.
		unsigned int depth;

		/*
		 * The score that was recorded in the position. How this
		 * is interpreted depends in the entry type.
		 */
		int score;

		/*
		 * This flag determines how the entry can be replaced. If the entry
		 * is useful it will not be replaced.
		 */
		bool usefulEntry;

		/*
		 * The best move at this position.
		 */
		move bestMove;

		/*
		 * Creates an uninitialized entry.
		 */
		transposition_entry() : hashKey(0), entryType(uninitialized), depth(0), score(0), usefulEntry(false), bestMove(NULLMOVE) {}

		//constructor from values
		transposition_entry(uint64 hashKey, unsigned short entryType, unsigned int depth, int score, bool useful, const move& m)
			: hashKey(hashKey), entryType(entryType), depth(depth), score(score), usefulEntry(useful), bestMove(m) {}

		//Copy constructor.
		transposition_entry(const transposition_entry& other) : hashKey(other.hashKey), entryType(other.entryType), depth(other.depth),
				score(other.score), usefulEntry(other.usefulEntry), bestMove(other.bestMove) {}

		//Assignment operator.
		void operator=(const transposition_entry& other) {
			hashKey = other.hashKey;
			entryType = other.entryType;
			depth = other.depth;
			score = other.score;
			usefulEntry = other.usefulEntry;
			bestMove = other.bestMove;
		}

		bool operator==(const transposition_entry& other) {
			return hashKey == other.hashKey && entryType == other.entryType && depth == other.depth &&
					score == other.score && usefulEntry == other.usefulEntry && bestMove == other.bestMove;
		}

		bool operator!=(const transposition_entry& other) {
			return !(*this == other);
		}
	};

	/*
	 * A constant entry that is used for "uninitialized" entries.
	 */
	extern const transposition_entry EMPTY_ENTRY;

	/*
	 * The amount of entries in the transposition table. Determined by how many entries can
	 * fit in 256 MB.
	 */
	extern const unsigned int def_transposition_table_size;

	/*
	 * Transposition table implementation, that is a fixed size hash map. Sadly unordered_map is
	 * not good, because it can't have a fixed size.
	 * - Key: must be a type for which mod operator is defined.
	 */
	class transposition_table {

		/*
		 * Transposition table size.
		 */
		const unsigned int size;

		/*
		 * Entry array with fixed size. Each entry is "initialized" to have uninitialized type.
		 */
		std::vector<transposition_entry> entries;

		unsigned int tt_lookup_count;

		unsigned int tt_put_count;

	public:
		/*
		 * Create an "empty" transposition table. It will have all entries set to EMPTY_ENTRY.
		 */
		transposition_table(unsigned int size) : size(size), entries(size, EMPTY_ENTRY), tt_lookup_count(0), tt_put_count(0) {}

		/*
		 * Returns the entry for the given key. First the index of the key
		 * is created using the modulo operator.
		 * To see if there is an actual entry, the result must be compared with
		 * Value::EMPTY_ENTRY.
		 */
		transposition_entry& find(uint64 key) {
			++tt_lookup_count;
			return entries[key % size];
		}

		/*
		 * Adds a new entry to the table, according to the replacement
		 * strategy.
		 */
		void put(uint64 key, transposition_entry entry) {
			auto mod = key % size;
			if(replaceOldEntry(entries[mod], entry)) {
				++tt_put_count;
				//replace the old entry
				entries[mod] = entry;
			}
		}

		/*
		 * Marks all entries as "useless". This is called between each move search, to
		 * allow the replacement of old entries.
		 */
		void invalidateEntries() {
			for(unsigned int i = 0; i < size; ++i) {
				entries[i].usefulEntry = false;
			}
		}

		void printDebug() const {
			std::cout << "TT lookup used: " << tt_lookup_count << "\n" <<
					"TT put used: " << tt_put_count << std::endl;
		}

	private:

		/*
		 * This function determines the replacement strategy of the transposition table.
		 */
		inline bool replaceOldEntry(transposition_entry oldEntry, transposition_entry newEntry) {
			if(oldEntry.entryType == exact && newEntry.entryType != exact) return false; //never replace exact with not exact
			if(oldEntry.entryType != exact && newEntry.entryType == exact) return true; //always replace not exact with exact
			return !oldEntry.usefulEntry; //if the old entry was used in lookup recently, then dont replace
		}
	};
}

#endif /* SRC_ENGINE_TRANSPOSITION_TABLE_H_ */
