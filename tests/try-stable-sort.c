/*
  Copyright © 2022 Barry Schwartz

  This program is free software: you can redistribute it and/or
  modify it under the terms of the GNU General Public License, as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received copies of the GNU General Public License
  along with this program. If not, see
  <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <patience-sort.h>

/*------------------------------------------------------------------*/
/* A simple linear congruential generator.                          */

/* The multiplier LCG_A comes from Steele, Guy; Vigna, Sebastiano (28
   September 2021). "Computationally easy, spectrally good multipliers
   for congruential pseudorandom number generators".
   arXiv:2001.05304v3 [cs.DS] */
#define LCG_A UINT64_C(0xf1357aea2e62a9c5)

/* LCG_C must be odd. */
#define LCG_C UINT64_C(0xbaceba11beefbead)

uint64_t seed = 0;

static double
random_double (void)
{
  /* IEEE "binary64" or "double" has 52 bits of precision. We will
     take the high 48 bits of the seed and divide it by 2**48, to get
     a number 0.0 <= randnum < 1.0 */
  const double high_48_bits = (double) (seed >> 16);
  const double divisor = (double) (UINT64_C (1) << 48);
  const double randnum = high_48_bits / divisor;

  /* The following operation is modulo 2**64, by virtue of standard C
     behavior for uint64_t. */
  seed = (LCG_A * seed) + LCG_C;

  return randnum;
}

static int
random_int (int m, int n)
{
  return m + (int) (random_double () * (n - m + 1));
}

/*------------------------------------------------------------------*/

#define CHECK(expr)                             \
  if (expr)                                     \
    {}                                          \
  else                                          \
    check_failed (__FILE__, __LINE__)

static void
check_failed (const char *file, unsigned int line)
{
  fprintf (stderr, "CHECK failed at %s:%u\n", file, line);
  exit (1);
}

static int
first_letter_cmp (const void *px, const void *py)
{
  const char *x = *((const char **) px);
  const char cx = tolower (x[0]);
  const char *y = *((const char **) py);
  const char cy = tolower (y[0]);
  return ((cx < cy) ? -1 : ((cx > cy) ? 1 : 0));
}

static size_t
find_first_letter (size_t n,
                   const char *words_copy[n],
                   int first_letter)
{
  size_t i = 0;
  while (i != n && (words_copy[i] == NULL
                    || tolower (words_copy[i][0]) != first_letter))
    i += 1;
  return i;
}

static void
scramble_words (size_t n,
                const char *words[n],
                const char *scrambled_words[n])
{
  /* Scramble the words, without changing the relative order of words
     that start with the same letter. */

  int num_letters = 26;
  int letters[26] =
    { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
      'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
      's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

  const char *words_copy[n];
  for (size_t i = 0; i != n; i += 1)
    words_copy[i] = words[i];

  size_t i_scrambled = 0;
  while (i_scrambled != n)
    {
      const int i_first_letter = random_int (0, num_letters - 1);
      const int first_letter = letters[i_first_letter];
      size_t j = find_first_letter (n, words_copy, first_letter);
      if (j != n)
        {
          /* Move the word. */
          scrambled_words[i_scrambled] = words_copy[j];
          i_scrambled += 1;
          words_copy[j] = NULL;
        }
      else
        {
          /* Remove the letter. */
          for (int i = i_first_letter; i < num_letters - 1; i += 1)
            letters[i] = letters[i + 1];
          num_letters -= 1;
        }
    }
}

static void
test_stable_sort (void)
{
  const char *words[1000] =
    { "a", "ability", "able", "about", "above", "accept", "according",
"account", "across", "act", "action", "activity", "actually", "add", "address",
"administration", "admit", "adult", "affect", "after", "again", "against", "age",
"agency", "agent", "ago", "agree", "agreement", "ahead", "air", "all", "allow",
"almost", "alone", "along", "already", "also", "although", "always", "American",
"among", "amount", "analysis", "and", "animal", "another", "answer", "any", "anyone",
"anything", "appear", "apply", "approach", "area", "argue", "arm", "around", "arrive",
"art", "article", "artist", "as", "ask", "assume", "at", "attack", "attention",
"attorney", "audience", "author", "authority", "available", "avoid", "away", "baby",
"back", "bad", "bag", "ball", "bank", "bar", "base", "be", "beat", "beautiful",
"because", "become", "bed", "before", "begin", "behavior", "behind", "believe",
"benefit", "best", "better", "between", "beyond", "big", "bill", "billion", "bit",
"black", "blood", "blue", "board", "body", "book", "born", "both", "box", "boy",
"break", "bring", "brother", "budget", "build", "building", "business", "but", "buy",
"by", "call", "camera", "campaign", "can", "cancer", "candidate", "capital", "car",
"card", "care", "career", "carry", "case", "catch", "cause", "cell", "center",
"central", "century", "certain", "certainly", "chair", "challenge", "chance", "change",
"character", "charge", "check", "child", "choice", "choose", "church", "citizen",
"city", "civil", "claim", "class", "clear", "clearly", "close", "coach", "cold",
"collection", "college", "color", "come", "commercial", "common", "community",
"company", "compare", "computer", "concern", "condition", "conference", "Congress",
"consider", "consumer", "contain", "continue", "control", "cost", "could", "country",
"couple", "course", "court", "cover", "create", "crime", "cultural", "culture", "cup",
"current", "customer", "cut", "dark", "data", "daughter", "day", "dead", "deal",
"death", "debate", "decade", "decide", "decision", "deep", "defense", "degree",
"Democrat", "democratic", "describe", "design", "despite", "detail", "determine",
"develop", "development", "die", "difference", "different", "difficult", "dinner",
"direction", "director", "discover", "discuss", "discussion", "disease", "do", "doctor",
"dog", "door", "down", "draw", "dream", "drive", "drop", "drug", "during", "each",
"early", "east", "easy", "eat", "economic", "economy", "edge", "education", "effect",
"effort", "eight", "either", "election", "else", "employee", "end", "energy", "enjoy",
"enough", "enter", "entire", "environment", "environmental", "especially", "establish",
"even", "evening", "event", "ever", "every", "everybody", "everyone", "everything",
"evidence", "exactly", "example", "executive", "exist", "expect", "experience",
"expert", "explain", "eye", "face", "fact", "factor", "fail", "fall", "family", "far",
"fast", "father", "fear", "federal", "feel", "feeling", "few", "field", "fight",
"figure", "fill", "film", "final", "finally", "financial", "find", "fine", "finger",
"finish", "fire", "firm", "first", "fish", "five", "floor", "fly", "focus", "follow",
"food", "foot", "for", "force", "foreign", "forget", "form", "former", "forward",
"four", "free", "friend", "from", "front", "full", "fund", "future", "game", "garden",
"gas", "general", "generation", "get", "girl", "give", "glass", "go", "goal", "good",
"government", "great", "green", "ground", "group", "grow", "growth", "guess", "gun",
"guy", "hair", "half", "hand", "hang", "happen", "happy", "hard", "have", "he", "head",
"health", "hear", "heart", "heat", "heavy", "help", "her", "here", "herself", "high",
"him", "himself", "his", "history", "hit", "hold", "home", "hope", "hospital", "hot",
"hotel", "hour", "house", "how", "however", "huge", "human", "hundred", "husband", "I",
"idea", "identify", "if", "image", "imagine", "impact", "important", "improve", "in",
"include", "including", "increase", "indeed", "indicate", "individual", "industry",
"information", "inside", "instead", "institution", "interest", "interesting",
"international", "interview", "into", "investment", "involve", "issue", "it", "item",
"its", "itself", "job", "join", "just", "keep", "key", "kid", "kill", "kind", "kitchen",
"know", "knowledge", "land", "language", "large", "last", "late", "later", "laugh",
"law", "lawyer", "lay", "lead", "leader", "learn", "least", "leave", "left", "leg",
"legal", "less", "let", "letter", "level", "lie", "life", "light", "like", "likely",
"line", "list", "listen", "little", "live", "local", "long", "look", "lose", "loss",
"lot", "love", "low", "machine", "magazine", "main", "maintain", "major", "majority",
"make", "man", "manage", "management", "manager", "many", "market", "marriage",
"material", "matter", "may", "maybe", "me", "mean", "measure", "media", "medical",
"meet", "meeting", "member", "memory", "mention", "message", "method", "middle",
"might", "military", "million", "mind", "minute", "miss", "mission", "model", "modern",
"moment", "money", "month", "more", "morning", "most", "mother", "mouth", "move",
"movement", "movie", "Mr", "Mrs", "much", "music", "must", "my", "myself", "name",
"nation", "national", "natural", "nature", "near", "nearly", "necessary", "need",
"network", "never", "new", "news", "newspaper", "next", "nice", "night", "no", "none",
"nor", "north", "not", "note", "nothing", "notice", "now", "n't", "number", "occur",
"of", "off", "offer", "office", "officer", "official", "often", "oh", "oil", "ok",
"old", "on", "once", "one", "only", "onto", "open", "operation", "opportunity",
"option", "or", "order", "organization", "other", "others", "our", "out", "outside",
"over", "own", "owner", "page", "pain", "painting", "paper", "parent", "part",
"participant", "particular", "particularly", "partner", "party", "pass", "past",
"patient", "pattern", "pay", "peace", "people", "per", "perform", "performance",
"perhaps", "period", "person", "personal", "phone", "physical", "pick", "picture",
"piece", "place", "plan", "plant", "play", "player", "PM", "point", "police", "policy",
"political", "politics", "poor", "popular", "population", "position", "positive",
"possible", "power", "practice", "prepare", "present", "president", "pressure",
"pretty", "prevent", "price", "private", "probably", "problem", "process", "produce",
"product", "production", "professional", "professor", "program", "project", "property",
"protect", "prove", "provide", "public", "pull", "purpose", "push", "put", "quality",
"question", "quickly", "quite", "race", "radio", "raise", "range", "rate", "rather",
"reach", "read", "ready", "real", "reality", "realize", "really", "reason", "receive",
"recent", "recently", "recognize", "record", "red", "reduce", "reflect", "region",
"relate", "relationship", "religious", "remain", "remember", "remove", "report",
"represent", "Republican", "require", "research", "resource", "respond", "response",
"responsibility", "rest", "result", "return", "reveal", "rich", "right", "rise", "risk",
"road", "rock", "role", "room", "rule", "run", "safe", "same", "save", "say", "scene",
"school", "science", "scientist", "score", "sea", "season", "seat", "second", "section",
"security", "see", "seek", "seem", "sell", "send", "senior", "sense", "series",
"serious", "serve", "service", "set", "seven", "several", "sex", "sexual", "shake",
"share", "she", "shoot", "short", "shot", "should", "shoulder", "show", "side", "sign",
"significant", "similar", "simple", "simply", "since", "sing", "single", "sister",
"sit", "site", "situation", "six", "size", "skill", "skin", "small", "smile", "so",
"social", "society", "soldier", "some", "somebody", "someone", "something", "sometimes",
"son", "song", "soon", "sort", "sound", "source", "south", "southern", "space", "speak",
"special", "specific", "speech", "spend", "sport", "spring", "staff", "stage", "stand",
"standard", "star", "start", "state", "statement", "station", "stay", "step", "still",
"stock", "stop", "store", "story", "strategy", "street", "strong", "structure",
"student", "study", "stuff", "style", "subject", "success", "successful", "such",
"suddenly", "suffer", "suggest", "summer", "support", "sure", "surface", "system",
"table", "take", "talk", "task", "tax", "teach", "teacher", "team", "technology",
"television", "tell", "ten", "tend", "term", "test", "than", "thank", "that", "the",
"their", "them", "themselves", "then", "theory", "there", "these", "they", "thing",
"think", "third", "this", "those", "though", "thought", "thousand", "threat", "three",
"through", "throughout", "throw", "thus", "time", "to", "today", "together", "tonight",
"too", "top", "total", "tough", "toward", "town", "trade", "traditional", "training",
"travel", "treat", "treatment", "tree", "trial", "trip", "trouble", "true", "truth",
"try", "turn", "TV", "two", "type", "under", "understand", "unit", "until", "up",
"upon", "us", "use", "usually", "value", "various", "very", "victim", "view",
"violence", "visit", "voice", "vote", "wait", "walk", "wall", "want", "war", "watch",
"water", "way", "we", "weapon", "wear", "week", "weight", "well", "west", "western",
"what", "whatever", "when", "where", "whether", "which", "while", "white", "who",
"whole", "whom", "whose", "why", "wide", "wife", "will", "win", "wind", "window",
"wish", "with", "within", "without", "woman", "wonder", "word", "work", "worker",
"world", "worry", "would", "write", "writer", "wrong", "yard", "yeah", "year", "yes",
"yet", "you", "young", "your", "yourself" };

  const char *scrambled_words[1000];
  scramble_words (1000, words, scrambled_words);

  const char *sorted_words[1000];
  patience_sort (scrambled_words, 1000, sizeof (const char *),
                 first_letter_cmp, sorted_words);

  /* sorted_words should be in the original order. */
  for (size_t i = 0; i != 1000; i += 1)
    CHECK (strcmp (sorted_words[i], words[i]) == 0);
}

int
main (int argc, char *argv[])
{
  (void) random_double ();
  (void) random_double ();
  (void) random_double ();
  (void) random_double ();
  (void) random_double ();
  test_stable_sort ();
  return 0;
}
