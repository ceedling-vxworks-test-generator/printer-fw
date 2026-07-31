const std::vector<RIMDataDefinition>&
SkeletonDataDefinitionProvider::GetDefinitions() const
{
    static const std::vector<RIMDataDefinition>
    definitions =
    {
        {
            RIMDataId::kTemperatureSensorA,
            DataDomain::kDevice,
            ValueType::kDouble,
            "Temperature"
        }
    };

    return definitions;
}