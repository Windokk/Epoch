#include "audio_source.hpp"

#include "engine/ecs/objects/actors/actor.hpp"


namespace EPOCH::Engine::ECS::Components{
    
    void AudioSource::Update()
    {
        if(audioID.IsValid() && activated){
            Audio::AudioManager::GetInstance().UpdateSound(audioID, parent->transform->GetPosition(), volume);
        }
        else{
            if(path != nullptr && volume != -1.0f){
                audioID = Audio::AudioIDManager::GenerateNewID();
                Audio::AudioManager::GetInstance().CreateSound(audioID, path->full, parent->transform->GetPosition());

            }
        }
    }

    AudioSource::AudioSource(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
    }

    void AudioSource::SetPath(std::string path)
    {
        this->path = std::make_unique<Filesystem::Path>(path);
        Update();
    }

    void AudioSource::SetVolume(float volume)
    {
        this->volume = volume;
        Update();
    }

    void AudioSource::Play()
    {
        if(!activated)
            return;

        Audio::AudioManager::GetInstance().PlaySound(this->audioID, this->volume);
    }

    void AudioSource::Pause()
    {
        Audio::AudioManager::GetInstance().PauseSound(this->audioID);
    }

    void AudioSource::RemoveSound()
    {
        Audio::AudioManager::GetInstance().RemoveSound(this->audioID);
    }
}