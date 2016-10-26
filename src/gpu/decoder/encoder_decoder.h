#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

#include "common/scorer.h"
#include "common/loader.h"
#include "common/base_best_hyps.h"

#include "common/threadpool.h"
#include <thrust/device_vector.h>

namespace GPU {

class EncoderDecoderState;
class Encoder;
class Decoder;
class Weights;

namespace mblas {
  template <class VecType>
  class TMatrix;
  typedef TMatrix<thrust::device_vector<float>> Matrix;
}

////////////////////////////////////////////
class EncoderDecoder : public Scorer {
  private:
    typedef EncoderDecoderState EDState;

  public:
    EncoderDecoder(const std::string& name,
                   const YAML::Node& config,
                   size_t tab,
                   const Weights& model);

    virtual void Score(const State& in, State& out);

    virtual State* NewState();

    virtual void BeginSentenceState(State& state);

    virtual void SetSource(const Sentence& source);

    virtual void AssembleBeamState(const State& in,
                                   const Beam& beam,
                                   State& out);

    void GetAttention(mblas::Matrix& Attention);

    mblas::Matrix& GetAttention();
    virtual BaseMatrix& GetProbs();

    size_t GetVocabSize() const;

    void Filter(const std::vector<size_t>& filterIds);

    virtual ~EncoderDecoder();

  private:
    const Weights& model_;
    std::unique_ptr<Encoder> encoder_;
    std::unique_ptr<Decoder> decoder_;
    thrust::device_vector<size_t> indeces_;

    std::unique_ptr<mblas::Matrix> SourceContext_;
};

////////////////////////////////////////////
class EncoderDecoderLoader : public Loader {
  public:
    EncoderDecoderLoader(const std::string name,
                         const YAML::Node& config);

    virtual void Load();

    virtual ScorerPtr NewScorer(size_t taskId);
    virtual BestHypsType GetBestHyps();

  private:
    std::vector<std::unique_ptr<Weights>> weights_;
};

}
