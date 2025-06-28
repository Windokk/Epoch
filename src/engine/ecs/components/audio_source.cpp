#include "audio_source.hpp"

#include "engine/ecs/objects/actors/actor.hpp"


namespace SHAME::Engine::ECS::Components{
    
    void AudioSource::Update()
    {
        if(audioID.IsValid()){
            Audio::AudioManager::UpdateSound(audioID, parent->GetComponent<ECS::Components::Transform>().GetPosition(), volume);
        }
        else{
            if(path != nullptr && volume != -1.0f){
                audioID = Audio::AudioIDManager::GenerateNewID();
                Audio::AudioManager::CreateSound(audioID, path->full, parent->GetComponent<ECS::Components::Transform>().GetPosition());

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
        Audio::AudioManager::PlaySound(this->audioID, this->volume);
    }

    void AudioSource::Pause()
    {
        Audio::AudioManager::PauseSound(this->audioID);
    }

    void AudioSource::RemoveSound()
    {
        Audio::AudioManager::RemoveSound(this->audioID);
    }
}